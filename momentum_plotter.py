import matplotlib.pyplot as plt

def plot_momentum(log_file_path):
    log_file = open(log_file_path, 'r')
    log_lines = log_file.readlines()
    log_file.close()

    # Extract the momentum values from the log file (one value per line)
    momentum_values = []
    for line in log_lines:
        if line.strip():  # Check if the line is not empty
            try:
                momentum = float(line.strip())
                momentum_values.append(momentum)
            except ValueError:
                print(f"Skipping invalid line: {line.strip()}")
                continue

    # Plot the momentum values
    plt.figure(figsize=(10, 5))
    plt.plot(momentum_values, marker='o', linestyle='-', color='b')
    plt.title('Momentum Values Over Time')
    plt.xlabel('Iteration')
    plt.ylabel('Momentum Value')
    plt.grid()
    plt.axhline(y=0, color='r', linestyle='--', label='Zero Line')
    plt.legend()
    plt.show()

if __name__ == "__main__":
    log_file_path = 'momentum_log.txt'  # Path to the log file
    plot_momentum(log_file_path)

    log_file_path2 = 'angular_momentum_log.txt'  # Path to the log file
    plot_momentum(log_file_path2)