#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <utility> // For std::pair

// RK4 Differential Solver Class
class DifferentialSolver {
private:
    // RK4 method implementation
    static double rk4Step(double (*f)(double, double), double x, double y, double h) {
        double k1 = f(x, y);
        double k2 = f(x + h / 2, y + h * k1 / 2);
        double k3 = f(x + h / 2, y + h * k2 / 2);
        double k4 = f(x + h, y + h * k3);
        return y + (h / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
    }

public:
	// Solve the differential equation using RK4 method
    static std::vector<std::pair<double, double>> solve(
        double (*f)(double, double), // The differential equation dy/dx = f(x, y)
        double x0,                   // Initial x value
        double y0,                   // Initial y value
        double xEnd,                 // Final x value
        double stepSize              // Step size (h)
    ) {
        std::vector<std::pair<double, double>> solution;
        double x = x0, y = y0;

        // Store the initial point
        solution.push_back(std::make_pair(x, y));

        while (x < xEnd) {
            double h = std::min(stepSize, xEnd - x); // Adjust step size
            y = rk4Step(f, x, y, h); // Calculate next y using RK4
            x += h;                  // Increment x
            solution.push_back(std::make_pair(x, y)); // Store the point
        }

        return solution;
    }
};

// Predefined equations (dy/dx = f(x, y))
double linear(double x, double y) { return x + y; }
double decay(double x, double y) { return -y; }
double harmonic(double x, double y) { return -x; }
double growth(double x, double y) { return y; }
double nonlinear(double x, double y) { return x * x + y * y; }
double trigonometric(double x, double y) { return sin(x) * y; }

// Helper function to display the solution
void displaySolution(const std::vector<std::pair<double, double>>& solution) {
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nSolution:\n";
    std::cout << "x\t\ty\n";
    for (const auto& point : solution) {
        std::cout << point.first << "\t\t" << point.second << "\n";
    }
}

// Helper function to get numerical input from the user
double getNumberInput(const std::string& prompt, double min, double max) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) {
            return value;
        }
        std::cout << "Invalid input. Please enter a value between " << min << " and " << max << ".\n";
    }
}

// User interface to handle interaction
void runInterface() {
    while (true) {
        std::cout << "\n=== Differential Equation Solver ===\n";
        std::cout << "1. Linear (dy/dx = x + y)\n";
        std::cout << "2. Exponential Decay (dy/dx = -y)\n";
        std::cout << "3. Harmonic Oscillator (dy/dx = -x)\n";
        std::cout << "4. Exponential Growth (dy/dx = y)\n";
        std::cout << "5. Nonlinear (dy/dx = x^2 + y^2)\n";
        std::cout << "6. Trigonometric (dy/dx = sin(x) * y)\n";
        std::cout << "7. Exit\n";
        std::cout << "Choose an equation to solve (1-7): ";

        int choice;
        std::cin >> choice;

        if (choice == 7) break;

        // Select the equation based on user choice
        double (*equation)(double, double) = nullptr;
        switch (choice) {
            case 1: equation = linear; break;
            case 2: equation = decay; break;
            case 3: equation = harmonic; break;
            case 4: equation = growth; break;
            case 5: equation = nonlinear; break;
            case 6: equation = trigonometric; break;
            default:
                std::cout << "Invalid choice. Please select a valid option.\n";
                continue;
        }

        // Input parameters for solving
        double x0 = getNumberInput("Enter initial x value: ", -1000.0, 1000.0);
        double y0 = getNumberInput("Enter initial y value: ", -1000.0, 1000.0);
        double xEnd = getNumberInput("Enter final x value: ", x0, 1000.0);
        double stepSize = getNumberInput("Enter step size (0.001-1.0): ", 0.001, 1.0);

        // Solve the differential equation
        auto solution = DifferentialSolver::solve(equation, x0, y0, xEnd, stepSize);

        // Display the solution
        displaySolution(solution);
    }
}

int main() {
    runInterface();
    return 0;
}