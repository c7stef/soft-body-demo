#include "graf.hpp"

#include "Line.hpp"
#include "utilities.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include "tinyfiledialogs.h"

void Graf::update()
{
    for (auto& nod : noduri)
        nod.update();

    for (auto& nodeInfo : edgeInfo)
        for (auto& info : nodeInfo.second)
            info.second.update();
}

void Graf::loadFromFile(std::string filename)
{
    std::ifstream file { filename };

    int nodeCount {};
    file >> nodeCount;

    noduri.clear();

    for (int nodeVal = 1; nodeVal <= nodeCount; nodeVal++)
    {
        Nod newNod { nodeVal };
        newNod.setPosition(
            Util::windowSize.x / 2.0f + 100 * std::cos(nodeVal * 2.0f * M_PI / nodeCount),
            Util::windowSize.y / 2.0f - 100 * std::sin(nodeVal * 2.0f * M_PI / nodeCount)
        );
        noduri.push_back(std::move(newNod));
    }

    adiacenta.clear();

    for (int i = 0; i < nodeCount; i++)
    {
        std::vector<int> randAdiacenta (nodeCount);
        for (int i = 0; i < nodeCount; i++)
            file >> randAdiacenta[i];
        adiacenta.push_back(std::move(randAdiacenta));
    }

    file.close();

    edgeInfo.clear();

    for (int i = 0; i < nodeCount; i++)
    {
        for (int j = 0; j < nodeCount; j++)
        {
            // If the nodes are adjacent, insert empty EdgeInfo at [i, j] and [j, i]
            if (adiacenta[i][j])
                edgeInfo[i][j] = {};
        }
    }
}

void Graf::openFileDialogAndLoad()
{
    auto filename{tinyfd_openFileDialog(
        "Select graph description file",
        NULL,   // No default path
        0,      // Zero filter patterns
        NULL,   // No filter pattern array
        NULL,   // No filter description
        0       // Don't allow multiple selects
    )};

    if (filename)
        loadFromFile(filename);
}

void Graf::selectEdge(int x, int y)
{
    edgeInfo[x][y].highlight();
    edgeInfo[y][x].highlight();
}

void Graf::deselectEdge(int x, int y)
{
    edgeInfo[x][y].unhighlight();
    edgeInfo[y][x].unhighlight();
}

void Graf::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    for (int i = 0; i < static_cast<NoduriSSize>(noduri.size()); i++)
        for (int j = i + 1; j < static_cast<NoduriSSize>(noduri.size()); j++)
            if (adiacenta[i][j])
            {
                sw::Line muchie {
                    noduri[i].getPosition(),
                    noduri[j].getPosition(),
                    edgeInfo.at(i).at(j).thickness(),
                    edgeInfo.at(i).at(j).color()
                };

                target.draw(muchie);
            }

    for (const auto& nod : noduri)
        target.draw(nod);
}

void Graf::EdgeInfo::highlight()
{
     targetColor = { 255, 0, 0, 120 };
     targetThickness = 10.0f;
}

void Graf::EdgeInfo::unhighlight()
{
     targetColor = { 0, 0, 0, 40 };
     targetThickness = 4.0f;
}

void Graf::EdgeInfo::update()
{
    currentColor = Util::lerp(currentColor, targetColor, smoothness);
    currentThickness = Util::lerp(currentThickness, targetThickness, smoothness);
}
