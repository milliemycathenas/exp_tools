#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

struct Level {
    int index;
    float energy;
    float uncertainty;
    std::string spin;
};

struct Gamma {
    int index;
    float energy;
    float uncertainty;
    std::string multipolarity;
    int from_level;
    int to_level;
    float intensity;
};

std::string to_upper(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::vector<std::string> split_fields(const std::string& line) {
    std::istringstream iss(line);
    std::string word;
    std::vector<std::string> fields;
    while (iss >> word) fields.push_back(word);
    return fields;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: ./ensdf_to_ags <input.txt> <output.ags> <nuclide>\n";
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);
    std::string target_nuclide = to_upper(argv[3]);

    if (!infile || !outfile) {
        std::cerr << "File error.\n";
        return 1;
    }

    std::vector<Level> levels;
    std::vector<Gamma> gammas;
    std::map<float, int> energy_to_index;
    std::string line;
    int level_counter = 1, gamma_counter = 1;

    while (getline(infile, line)) {
        auto fields = split_fields(line);
        if (fields.size() < 3) continue;
        std::string nuc = to_upper(fields[0]);
        if (nuc != target_nuclide) continue;

        if (fields[1] == "L") {
            try {
                float energy = std::stof(fields[2]);
                float err = (fields.size() >= 4) ? std::stof(fields[3]) : 0.0f;
                std::string spin = (fields.size() >= 5) ? fields[4] : "";
                levels.push_back({level_counter, energy, err, spin});
                energy_to_index[energy] = level_counter++;
            } catch (...) { continue; }
        }
        else if (fields[1] == "G" && fields.size() >= 6) {
            try {
                float energy = std::stof(fields[2]);
                float err = std::stof(fields[3]);
                std::string mult = fields[4];
                float final_e = std::stof(fields[5]);
                float initial_e = final_e + energy;

                int from = energy_to_index.count(initial_e) ? energy_to_index[initial_e] : -1;
                int to = energy_to_index.count(final_e) ? energy_to_index[final_e] : -1;
                float intensity = (fields.size() > 6) ? std::stof(fields[6]) : 0.0f;

                if (from > 0 && to > 0) {
                    gammas.push_back({gamma_counter++, energy, err, mult, from, to, intensity});
                }
            } catch (...) { continue; }
        }
    }

    // Header
    outfile << "** ASCII Graphical Level Scheme file.\n";
    outfile << "** First five lines are reserved for comments.\n";
    outfile << "** Automatically generated\n";
    outfile << "** Program: ENSDF-to-AGS\n";
    outfile << "**\n";
    outfile << " ASCII GLS file format version 1.0\n";
    outfile << "**  Z Nlevels Ngammas  Nbands Nlabels CharSizeX CharSizeY ArrowWidFact\n";
    outfile << std::setw(5) << 0 << std::setw(9) << levels.size()
            << std::setw(9) << gammas.size() << std::setw(8) << 0 << std::setw(8) << 0
            << std::setw(10) << 40.0 << std::setw(10) << 45.0 << std::setw(10) << 3.0 << "\n";
    outfile << "**  MaxArrowTan MaxArrowDX DefBandWid DefBandSep ArrowWidth ArrowLength\n";
    outfile << std::setw(15) << 0.2679 << std::setw(11) << 999.0 << std::setw(11) << 350.0
            << std::setw(11) << 100.0 << std::setw(11) << 30.0 << std::setw(11) << 60.0 << "\n";
    outfile << "** ArrowBreak LevelBreak   LevCSX   LevCSY LevEnCSX LevEnCSY   GamCSX   GamCSY\n";
    outfile << std::setw(11) << 30.0 << std::setw(11) << 40.0
            << std::setw(9) << 40.0 << std::setw(9) << 45.0
            << std::setw(10) << 40.0 << std::setw(9) << 45.0
            << std::setw(9) << 40.0 << std::setw(9) << 45.0 << "\n";

    // Levels
    outfile << "** Level   Energy +/- err     Jpi     K Band# LevelFlag LabelFlag EnLabFlag\n";
    for (auto& lvl : levels) {
        outfile << std::setw(6) << lvl.index
                << std::fixed << std::setw(10) << std::setprecision(3) << lvl.energy
                << std::setw(8) << lvl.uncertainty
                << std::setw(9) << lvl.spin << "     0     1     0     0     0 &\n";
        outfile << "++      0.00      0.00      0.00      0.00      0.00      0.00\n";
    }

    // Gammas
    outfile << "** Gamma   Energy +/- err  Mult  ILev  FLev  Intensity +/- err\n";
    outfile << "++     ConvCoef +/- error      BrRatio +/- error     MixRatio +/- error\n";
    outfile << "++   GammaX1  GammaX2  LabelDX  LabelDY GammaFlag LabelFlag\n";
    for (auto& g : gammas) {
        outfile << std::setw(6) << g.index
                << std::fixed << std::setw(10) << std::setprecision(3) << g.energy
                << std::setw(8) << g.uncertainty
                << std::setw(6) << g.multipolarity
                << std::setw(7) << g.from_level
                << std::setw(7) << g.to_level
                << std::setw(11) << g.intensity << "  0.0000 &\n";
        outfile << "++  0.000E+00 0.000E+00  0.00000E+00 0.000E+00  0.00000E+00 0.000E+00 &\n";
        outfile << "++      0.00      0.00      0.00      0.00         0         0\n";
    }

    std::cout << "AGS file created: " << argv[2] << "\n";
    return 0;
}
