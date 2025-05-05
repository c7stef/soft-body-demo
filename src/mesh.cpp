#include "mesh.hpp"

#include "utilities.hpp"

#include "Line.hpp"
#include "tinyfiledialogs.h"
#include "opencv4/opencv2/opencv.hpp"
#include "PoissonGenerator.h"
#include "CGAL/Simple_cartesian.h"
#include "CGAL/Delaunay_triangulation_2.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_map>

void Mesh::update(float deltaTime)
{
    for (auto& nod : noduri)
        nod.update(deltaTime);

    for (auto& nodeInfo : edgeInfo)
        for (auto& info : nodeInfo.second)
            info.second.update(deltaTime);
}

void Mesh::loadRawAdjacency(std::string filename)
{
    std::ifstream file { filename };

    int nodeCount {};
    file >> nodeCount;

    noduri.clear();

    for (int nodeVal = 1; nodeVal <= nodeCount; nodeVal++) {
        Nod newNod { nodeVal, font };
        newNod.setPosition(
            Util::windowSize.x / 2.0f + 100 * std::cos(nodeVal * 2.0f * M_PI / nodeCount),
            Util::windowSize.y / 2.0f - 100 * std::sin(nodeVal * 2.0f * M_PI / nodeCount)
        );
        noduri.push_back(std::move(newNod));
    }

    adiacenta.clear();

    for (int i = 0; i < nodeCount; i++) {
        std::unordered_map<int, float> randAdiacenta{};

        int temp{};
        for (int i = 0; i < nodeCount; i++) {
            file >> temp;
            if (temp)
                randAdiacenta.at(i) = 170.0f;
        }

        adiacenta[i] = std::move(randAdiacenta);
    }

    file.close();

    edgeInfo.clear();

    for (int i = 0; i < nodeCount; i++)
        for (int j = 0; j < nodeCount; j++)
            // If the nodes are adjacent, insert empty EdgeInfo at [i, j] and [j, i]
            if (adiacenta[i].contains(j))
                edgeInfo[i][j] = {};
}

void Mesh::loadFromFile(std::string filename, float resolution)
{
    // Load image + find contour

    cv::Mat img = cv::imread(filename, cv::IMREAD_UNCHANGED);

    std::vector<cv::Mat> channels;
    cv::split(img, channels);

    cv::Mat alpha = channels[3];
    cv::Mat objectMask;
    cv::threshold(alpha, objectMask, 10, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(objectMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    std::cout << "Number of contours: " << contours.size() << std::endl;

    int contourIdx = 0;
    for (int i = 0; i < static_cast<int>(contours.size()); i++) {
        if (contours[i].size() > contours[contourIdx].size())
            contourIdx = i;
    }
    std::vector<cv::Point> simplified{contours[contourIdx][0]};
    for (const auto& contour : contours[contourIdx]) {
        auto xDiff = simplified.back().x - contour.x;
        auto yDiff = simplified.back().y - contour.y;
        if (xDiff * xDiff + yDiff * yDiff > resolution * resolution)
            simplified.push_back(contour);
    }
    
    // Check if last point is close to first
    auto xDiff = simplified.back().x - simplified[0].x;
    auto yDiff = simplified.back().y - simplified[0].y;
    if (xDiff * xDiff + yDiff * yDiff < resolution * resolution / 4)
        simplified.pop_back();

    std::cout << "Number of points: " << simplified.size() << std::endl;

    // Generate Poisson points

    cv::Rect bbox = cv::boundingRect(simplified);
    int area = bbox.width * bbox.height;
    int totalArea = img.cols * img.rows;
    std::cout << "Bounding box area: " << area << std::endl;
    std::cout << "Total area: " << totalArea << std::endl;

    float numPointsPerArea = pointDensity / (resolution * resolution);
    unsigned int numPoints = static_cast<int>(numPointsPerArea * totalArea);
    std::cout << "Number of points to generate: " << numPoints << std::endl;

    PoissonGenerator::DefaultPRNG generator;
    auto poissonPoints = PoissonGenerator::generatePoissonPoints(
        numPoints, generator, false
    );

    for (auto& point : poissonPoints) {
        point.x *= img.cols;
        point.y *= img.rows;
    }

    // Filter points inside the contour

    using Kernel = CGAL::Simple_cartesian<float>;

    std::vector<Kernel::Point_2> filteredPoints;

    for (const auto& point : simplified)
        filteredPoints.emplace_back(point.x, point.y);

    for (const auto& point : poissonPoints) {
        auto distToContour = cv::pointPolygonTest(simplified, cv::Point2f{point.x, point.y}, true);
        if (distToContour >= resolution * std::sqrt(3) / 3)
            filteredPoints.emplace_back(point.x, point.y);
    }

    std::cout << "Number of points inside contour: " << filteredPoints.size() - simplified.size() << std::endl;

    // Apply Delaunay triangulation

    CGAL::Delaunay_triangulation_2<Kernel> dt(filteredPoints.begin(), filteredPoints.end());
    
    noduri.clear();
    
    std::unordered_map<decltype(dt)::Vertex_handle, int> vertexIndex;
    int idx = 0;
    for (auto vit = dt.finite_vertices_begin(); vit != dt.finite_vertices_end(); ++vit) {
        Nod newNod { idx, font };
        newNod.setPosition(
            vit->point().x(),
            vit->point().y()
        );
        noduri.push_back(std::move(newNod));
        
        vertexIndex[vit] = idx++;
    }

    adiacenta.clear();
    for (int i = 0; i < static_cast<int>(noduri.size()); i++)
        adiacenta[i] = {};
    
    for (auto eit = dt.finite_edges_begin(); eit != dt.finite_edges_end(); ++eit) {
        auto fh = eit->first;
        int i = eit->second;
    
        auto v1 = fh->vertex((i + 1) % 3);
        auto v2 = fh->vertex((i + 2) % 3);

        auto p1 = v1->point();
        auto p2 = v2->point();

        cv::Point2f mid(
            0.5f * (p1.x() + p2.x()),
            0.5f * (p1.y() + p2.y())
        );

        if (cv::pointPolygonTest(simplified, mid, false) < 0)
            continue;
    
        int idx1 = vertexIndex[v1];
        int idx2 = vertexIndex[v2];
    
        adiacenta[idx1][idx2] = 1.f * Util::distance(
            noduri[idx1].getPosition(),
            noduri[idx2].getPosition()
        );
        adiacenta[idx2][idx1] = adiacenta[idx1][idx2];
    }

    for (auto fit = dt.faces_begin(); fit != dt.faces_end(); ++fit) {
        // Skip faces whose edges are not in the adjacency list
        auto v1 = vertexIndex[fit->vertex(0)];
        auto v2 = vertexIndex[fit->vertex(1)];
        auto v3 = vertexIndex[fit->vertex(2)];
        if (!adiacenta[v1].contains(v2) ||
            !adiacenta[v2].contains(v3) ||
            !adiacenta[v3].contains(v1))
            continue;
        
        triangleInfo.push_back({
            v1, v2, v3,
            Util::signedArea(
                noduri[v1].getPosition(),
                noduri[v2].getPosition(),
                noduri[v3].getPosition()
            )
        });
    }

    edgeInfo.clear();

    for (int i = 0; i < static_cast<int>(noduri.size()); i++)
        for (int j = 0; j < static_cast<int>(noduri.size()); j++)
            // If the nodes are adjacent, insert empty EdgeInfo at [i, j] and [j, i]
            if (isEdge(i, j))
                edgeInfo[i][j] = {};
    
    if (!image.loadFromFile(filename)) {
        std::cerr << "Failed to load image!\n";
        return;
    }

    for (int i = 0; i < static_cast<int>(noduri.size()); i++) {
        auto scaledPos = noduri[i].getPosition() * scale;
        noduri[i].setPosition(
            scaledPos
            + sf::Vector2f{Util::windowSize} / 2.0f
            - sf::Vector2f{static_cast<float>(bbox.width), static_cast<float>(bbox.height)} * scale / 2.0f
        );
        for (auto& [neigh, dist] : adiacenta[i])
            dist *= scale;
        controlPoints.push_back(scaledPos);
    }

    for (auto& tri : triangleInfo) {
        tri.restSignedArea *= scale * scale;
    }
}

void Mesh::openFileDialogAndLoad(float resolution)
{
    auto filename{tinyfd_openFileDialog(
        "Select mesh description file",
        NULL,   // No default path
        0,      // Zero filter patterns
        NULL,   // No filter pattern array
        NULL,   // No filter description
        0       // Don't allow multiple selects
    )};

    if (filename)
        loadFromFile(filename, resolution);
}

float Mesh::edgeLength(int x, int y) const
{
    if (adiacenta.at(x).contains(y))
        return adiacenta.at(x).at(y);
    else
        return 0.0f;
}

void Mesh::selectEdge(int x, int y)
{
    edgeInfo[x][y].highlight();
    edgeInfo[y][x].highlight();
}

void Mesh::deselectEdge(int x, int y)
{
    edgeInfo[x][y].unhighlight();
    edgeInfo[y][x].unhighlight();
}

static float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

static sf::Vector2f rotated(const sf::Vector2f& v) {
    return sf::Vector2f{-v.y, v.x};
}

static sf::Vector2f rigidMLS(const sf::Vector2f& v, const std::vector<sf::Vector2f>& p, const std::vector<sf::Vector2f>& q) {
    const float EPSILON = 1e-8f;
    size_t n = p.size();
    std::vector<float> w(n);
    float w_sum = 0.0f;
    sf::Vector2f p_star(0, 0), q_star(0, 0);

    for (size_t i = 0; i < n; ++i) {
        float d2 = std::pow(Util::distance(p[i] - v, {0, 0}), 2.0f);
        w[i] = 1.0f / std::max(d2, EPSILON);
        w_sum += w[i];
        p_star += w[i] * p[i];
        q_star += w[i] * q[i];
    }

    p_star /= w_sum;
    q_star /= w_sum;

    std::vector<sf::Vector2f> phat(n), qhat(n);
    for (size_t i = 0; i < n; ++i) {
        phat[i] = p[i] - p_star;
        qhat[i] = q[i] - q_star;
    }

    // float sum1 = 0.0f, sum2 = 0.0f;
    // for (size_t i = 0; i < n; i++) {
    //     sum1 += w[i] * dot(qhat[i], phat[i]);
    //     sum2 += w[i] * dot(qhat[i], rotated(phat[i]));
    // }

    // float mu = std::sqrt(sum1*sum1 + sum2*sum2);

    float mu = 0.f;
    for (size_t i = 0; i < n; ++i)
        mu += w[i] * dot(phat[i], phat[i]);

    sf::Vector2f result{0, 0};
    for (size_t i = 0; i < n; ++i) {
        sf::Vector2 qhat_A{
            qhat[i].x * dot(phat[i], v - p_star) + qhat[i].y * dot(-rotated(phat[i]), v - p_star),
            qhat[i].x * dot(phat[i], -rotated(v - p_star)) + qhat[i].y * dot(-rotated(phat[i]), - rotated(v - p_star))
        };
        result += qhat_A * w[i] / mu;
    }

    return q_star + result;
}

void Mesh::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    if (!showImage) {
        for (int i = 0; i < static_cast<NoduriSSize>(noduri.size()); i++) {
            for (int j = i + 1; j < static_cast<NoduriSSize>(noduri.size()); j++) {
                if (isEdge(i, j)) {
                    sw::Line muchie {
                        noduri[i].getPosition(),
                        noduri[j].getPosition(),
                        edgeInfo.at(i).at(j).thickness(),
                        edgeInfo.at(i).at(j).color()
                    };

                    target.draw(muchie);
                }
            }
        }

        for (const auto& nod : noduri)
            target.draw(nod);
    } else {
        std::vector<sf::Vector2f> displacedPoints{};
        for (const auto& nod : noduri)
            displacedPoints.push_back(nod.getPosition());
        
        sf::VertexArray mesh(sf::Triangles);
        float width = image.getSize().x * scale;
        float height = image.getSize().y * scale;
        const int rows = static_cast<int>(width / meshImageSpacing);
        const int cols = static_cast<int>(height / meshImageSpacing);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::Vector2f tl(j * width / cols, i * height / rows);
                sf::Vector2f tr((j + 1) * width / cols, i * height / rows);
                sf::Vector2f bl(j * width / cols, (i + 1) * height / rows);
                sf::Vector2f br((j + 1) * width / cols, (i + 1) * height / rows);

                sf::Vector2f dtl = rigidMLS(tl, controlPoints, displacedPoints);
                sf::Vector2f dtr = rigidMLS(tr, controlPoints, displacedPoints);
                sf::Vector2f dbl = rigidMLS(bl, controlPoints, displacedPoints);
                sf::Vector2f dbr = rigidMLS(br, controlPoints, displacedPoints);

                mesh.append(sf::Vertex(dtl, tl / scale));
                mesh.append(sf::Vertex(dtr, tr / scale));
                mesh.append(sf::Vertex(dbl, bl / scale));

                mesh.append(sf::Vertex(dbl, bl / scale));
                mesh.append(sf::Vertex(dtr, tr / scale));
                mesh.append(sf::Vertex(dbr, br / scale));
            }
        }

        target.draw(mesh, &image);
    }
}

void Mesh::sendKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::I)
        showImage = !showImage;
}

void Mesh::EdgeInfo::highlight()
{
    targetColor = { 255, 0, 0, 120 };
    targetThickness = 10.0f;
}

void Mesh::EdgeInfo::unhighlight()
{
    targetColor = { 0, 0, 0, 40 };
    targetThickness = 4.0f;
}

void Mesh::EdgeInfo::update([[maybe_unused]] float deltaTime)
{
    currentColor = Util::lerp(currentColor, targetColor, smoothness);
    currentThickness = Util::lerp(currentThickness, targetThickness, smoothness);
}
