#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iomanip>

#pragma pack(push, 1)

struct Levdat {
    float e, de;
    float j, pi;
    float k;
    int   band;
    int   flg, slflg, elflg;
    float dxl, dxr, sldx, sldy, eldx, eldy;
    char  name[12];
    float x[4];
    char  sl[16];
    int   slnc;
    float slx, sly;
    char  el[16];
    int   elnc;
    float elx, ely;
};

struct Gamdat {
    int   li, lf;
    char  em;
    float e, de;
    float i, di;
    float br, a, d, da, dd, dbr, n;
    float x1, x2;
    int   flg, elflg;
    float eldx, eldy;
    float x[10], y[10];
    int   np;
    char  el[16];
    int   elnc;
    float elx, ely;
};

#pragma pack(pop)

struct Level {
    float energy;
    std::string spin;
};

struct Gamma {
    float energy;
    float intensity;
    int from, to;
    std::string multipolarity;
};

std::string to_upper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::vector<std::string> split_fields(const std::string& line) {
    std::istringstream iss(line);
    std::string field;
    std::vector<std::string> fields;
    while (iss >> field) fields.push_back(field);
    return fields;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: ./ensdf_to_gls_bin <input.txt> <output.gls> <nuclide>\n";
        return 1;
    }

    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2], std::ios::binary);
    std::string target_nuclide = to_upper(argv[3]);

    if (!infile || !outfile) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    std::vector<Level> levels;
    std::vector<Gamma> gammas;
    std::string line;

    while (getline(infile, line)) {
        auto fields = split_fields(line);
        if (fields.size() < 3) continue;

        std::string nuc = to_upper(fields[0]);
        if (nuc != target_nuclide) continue;

        if (fields[1] == "L") {
            try {
                float energy = std::stof(fields[2]);
                std::string spin = (fields.size() > 4 ? fields[4] : "");
                levels.push_back({energy, spin});
            } catch (...) {}
        } else if (fields[1] == "G") {
            try {
                float energy = std::stof(fields[2]);
                float intensity = (fields.size() > 3 ? std::stof(fields[3]) : 0.0f);
                std::string mult = (fields.size() > 4 ? fields[4] : "");
                if (levels.size() >= 2) {
                    int to = levels.size() - 1;
                    int from = to - 1;
                    gammas.push_back({energy, intensity, from, to, mult});
                }
            } catch (...) {}
        }
    }

    // Convert to binary GLS structures
    for (size_t i = 0; i < levels.size(); ++i) {
        Levdat lev = {};
        lev.e = levels[i].energy;
        lev.de = 0.0f;
        lev.j = 0.0f;
        lev.pi = 0.0f;
        lev.k = 0.0f;
        lev.band = 0;
        lev.flg = 0;
        lev.elflg = lev.slflg = 1;
        lev.dxl = lev.dxr = lev.sldx = lev.sldy = lev.eldx = lev.eldy = 0.0f;

        std::string name = "L" + std::to_string(i);
        strncpy(lev.name, name.c_str(), sizeof(lev.name) - 1);

        std::string slabel = levels[i].spin;
        strncpy(lev.sl, slabel.c_str(), sizeof(lev.sl) - 1);
        lev.slnc = static_cast<int>(slabel.length());
        lev.slx = lev.sly = 0.0f;

        std::string elabel = std::to_string((int)(levels[i].energy));
        strncpy(lev.el, elabel.c_str(), sizeof(lev.el) - 1);
        lev.elnc = static_cast<int>(elabel.length());
        lev.elx = lev.ely = 0.0f;

        outfile.write(reinterpret_cast<char*>(&lev), sizeof(Levdat));
    }

    for (const auto& g : gammas) {
        Gamdat gam = {};
        gam.li = g.from;
        gam.lf = g.to;
        gam.em = 'E';
        gam.e = g.energy;
        gam.i = g.intensity;
        gam.n = 1.0f;
        gam.np = 2;

        strncpy(gam.el, std::to_string((int)g.energy).c_str(), sizeof(gam.el) - 1);
        gam.elnc = static_cast<int>(strlen(gam.el));
        gam.elx = gam.ely = 0.0f;

        gam.x[0] = 0.0f; gam.y[0] = levels[g.from].energy;
        gam.x[1] = 0.0f; gam.y[1] = levels[g.to].energy;

        outfile.write(reinterpret_cast<char*>(&gam), sizeof(Gamdat));
    }

    std::cout << "Binary GLS file created: " << argv[2] << "\n";
    return 0;
}
