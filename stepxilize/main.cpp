#include <iostream>
#include <gmsh.h>
#include "rtree.h"
#include <cfloat>
#include <omp.h>
#include <thread>

#include <TopOpeBRepTool_SolidClassifier.hxx>

template<typename T>
struct vector3 {
    T x;
    T y;
    T z;

    vector3() {
        x = 0;
        y = 0;
        z = 0;
    };

    vector3(T _x, T _y, T _z) {
        x = _x;
        y = _y;
        z = _z;
    }
};

template<typename T>
std::ostream &operator<<(std::ostream &outs, const vector3<T> &p) {
    return outs << "(" << p.x << "," << p.y << "," << p.z << ")";
}

template<typename T>
const vector3<T> operator*(const vector3<T> &p, const double d) {
    return {p.x*d, p.y*d, p.z*d};
}

int main(int argc, char **argv) {
    gmsh::initialize();
    std::string file = "simple.step";

    if (argc > 1)
        file = argv[1];

    gmsh::model::add("model");

    std::vector<std::pair<int, int> > v;
    try {
        gmsh::model::occ::importShapes(file, v, true);
    } catch (...) {
        gmsh::logger::write("Could not load STEP file: bye!");
        gmsh::finalize();
        return -1;
    }

    std::cout << "model loaded. " << v.size() << " pairs" << std::endl;

    vector3<double> min{DBL_MAX, DBL_MAX, DBL_MAX};
    vector3<double> max{DBL_MIN, DBL_MIN, DBL_MIN};

    for (auto &pair : v) {
        vector3<double> lmin;
        vector3<double> lmax;
        gmsh::model::occ::getBoundingBox(pair.first, pair.second, lmin.x, lmin.y, lmin.z,
                                         lmax.x, lmax.y, lmax.z);

        min = {std::min(min.x, lmin.x), std::min(min.y, lmin.y), std::min(min.z, lmin.z)};
        max = {std::max(max.x, lmax.x), std::max(max.y, lmax.y), std::max(max.z, lmax.z)};
    }

    std::cout << "model bbox (x,y,z) = " << min << " - " << max << std::endl;

    gmsh::model::geo::synchronize();
    gmsh::model::occ::synchronize();

    double res = 5; //mm

    if (argc > 2)
        res = std::atof(argv[2]);

    const size_t nx = (max.x - min.x) / res;
    const size_t ny = (max.y - min.y) / res;
    const size_t nz = (max.z - min.z) / res;

    const double dx = (max.x - min.x) / nx;
    const double dy = (max.y - min.y) / ny;
    const double dz = (max.z - min.z) / nz;

    const double dx2 = dx / 2;
    const double dy2 = dy / 2;
    const double dz2 = dz / 2;

    std::cout << "nx/ny/nz " << nx << " / " << ny << " / " << nz << std::endl;
    std::cout << "dx/dy/dz " << dx << " / " << dy << " / " << dz << std::endl;

    int16_t *outM;

    outM = (int16_t *) calloc(sizeof(int16_t), nx * ny * nz);

    if(!outM){
        std::cout <<  "[ERROR] could not allocate " << nx*ny*nz*sizeof(int16_t) << " bytes" << std::endl;
        return -1;
    }

    for (auto &pair: v) {
        if (pair.first < 3)
            continue;

        std::cout << "====body " << pair.second << "====" << std::endl;

        vector3<double> lmin, lmax;
        gmsh::model::occ::getBoundingBox(pair.first, pair.second, lmin.x, lmin.y, lmin.z, lmax.x, lmax.y, lmax.z);

        std::vector<int> shells;
        std::vector<std::vector<int> > _faces;
        gmsh::model::occ::getSurfaceLoops(pair.second, shells, _faces);

        
        for (auto &shell: shells) {

        }

        int color[4] = {0,0,0,0};
        gmsh::model::getColor(pair.first, pair.second, color[0], color[1], color[2], color[3]);
        std::cout << "body color -> " << color[0] << "," << color[1] << "," << color[2] << std::endl;
        void *reg = gmsh::model::getRegionByTag(pair.second);
        /*if (!reg) {
            std::cout << "[FATAL!] reg is null" << std::endl;
            return -1;
        }*/

        size_t dones = 0;
#pragma omp parallel for
        for (size_t z = 0; z < nz; z++) {
            vector3<double> amin;

            amin.z = min.z + (z * dz);

            if (amin.z > lmax.z || amin.z < lmin.z)
                continue;

            for (size_t y = 0; y < ny; y++) {
                amin.y = min.y + (y * dy);

                if (amin.y > lmax.y || amin.y < lmin.y)
                    continue;

                std::vector<double> p1 = {lmin.x - dx2, amin.y + dx2, amin.z + dx2};
                std::vector<double> p2 = {lmax.x + dx2, amin.y + dx2, amin.z + dx2};

                std::vector<double> r;
                for (auto &shell: shells) {
                    auto tr = gmsh::model::faceLineInt(shell, p1, p2);
                    r.insert(r.end(), tr.begin(), tr.end());
                }

                if(r.empty())
                    continue;

                size_t x = 0;

                if (r.size() % 2 == 0) {
                    std::sort(r.begin(), r.end());

                    for (size_t ix = 0; ix < r.size(); ix += 2) {

                        //while (min.x + (x * dx) + dx2 < pnt)
                        //    x++;
                        x = ceil((r[ix] - min.x - dx2) / dx);
                        size_t x_dst = ceil((r[ix + 1] - min.x - dx2) / dx);
                        size_t start_addr = y * nx + z * nx * ny;

                        for(;x < x_dst; ++x){
                            if (outM[x + start_addr] > 0) {
                                std::cout << "WARN! (overlap)" << outM[x + start_addr] << " already in " << x
                                << "," << y << "," << z << std::endl;
                            }else{
                                outM[x + start_addr] = pair.second;
                            }
                        }
                    }
                } else {
                    std::cout << "WARN! odd intersection! " << r.size() << std::endl;
                    void *sc = gmsh::model::getNewSolidClassifier(reg);

                    for (; x < nx; x++) {
                        amin.x = min.x + (x * dx);

                        if (amin.x > lmax.x || amin.x < lmin.x)
                            continue;

                        //int inside = gmsh::model::isInside(pair.first, pair.second, {amin.x + dx2, amin.y + dy2, amin.z + dz2});
                        int inside = gmsh::model::isInside(reg, sc, {amin.x + dx2, amin.y + dy2, amin.z + dz2});

                        if (inside > 0) {
                            if (outM[x + y * nx + z * nx * ny] > 0) {
                                std::cout << "WARN! (overlap)" << outM[x + y * nx + z * nx * ny] << " already in " << x
                                << "," << y << "," << z << std::endl;
                                continue;
                            }

                            outM[x + y * nx + z * nx * ny] = pair.second;
                        }
                    }
                }
            }

            #pragma omp atomic
            ++dones;

            std::cout << "z " << (float)dones/nz*100 << "%" << std::endl;
            //delete static_cast<BRepClass3d_SolidClassifier *>(sc);
        }
    }

    gmsh::finalize();

    bool vxl = false;

    if(argc > 3)
        if(argv[3][0] == 'x')
            vxl = true;

    if(vxl){
        FILE* vxlOut = fopen("vxlout.vxl", "wb");

        double d = 0.3;
        char title[80];
        memset(title, ' ', 80);
        title[0] = 'a';
        int len = 80;
        int b;

        fwrite(&len, 4, 1, vxlOut);
        fwrite(title, 80, 1, vxlOut);
        fwrite(&len, 4, 1, vxlOut);

        len = 20;
        fwrite(&len, 4, 1, vxlOut);
        b = nx;
        fwrite(&b, 4, 1, vxlOut);
        b = ny;
        fwrite(&b, 4, 1, vxlOut);
        b = nz;
        fwrite(&b, 4, 1, vxlOut);
        b = v.size();
        fwrite(&b, 4, 1, vxlOut);
        fwrite(&b, 4, 1, vxlOut);
        fwrite(&len, 4, 1, vxlOut);

        len = 3*8;
        fwrite(&len, 4, 1, vxlOut);
        d = dx*0.1;
        fwrite(&d, 8, 1, vxlOut);
        d = dy*0.1;
        fwrite(&d, 8, 1, vxlOut);
        d = dz*0.1;
        fwrite(&d, 8, 1, vxlOut);
        fwrite(&len, 4, 1, vxlOut);

        len = nx*ny*nz*2;
        fwrite(&len, 4, 1, vxlOut);
        fwrite(outM, 2, nx * ny * nz, vxlOut);
        fwrite(&len, 4, 1, vxlOut);

        len = 2*v.size();
        uint16_t map[v.size()];
        int16_t q;
        for(auto& e : map)
            e = ++q;

        fwrite(&len, 4, 1, vxlOut);
        fwrite(map, 2*v.size(), 1, vxlOut);
        fwrite(&len, 4, 1, vxlOut);

        fclose(vxlOut);

        std::cout << "VOXELS RPP position " << min * 0.1 << std::endl;
    }else{
        FILE *fout = fopen("bufout", "wb");
        fwrite(outM, sizeof(int16_t), nx * ny * nz, fout);
        fclose(fout);

        free(outM);
    }
    return 0;
}
