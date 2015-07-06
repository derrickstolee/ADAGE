

#include "Grid.hpp"
#include "HexagonalGrid.hpp"
#include "Configuration.hpp"
#include "IdentifyingCodeConfiguration.hpp"
#include "GridAutomManager.hpp"

#include <stdio.h>
#include <string.h>

using namespace adage::grids;

int main(int argc, char** argv)
{
    Grid* grid = new HexagonalGrid(30);
    Configuration* conf = new IdentifyingCodeConfiguration(grid);

    GridAutomManager* manager = new GridAutomManager(grid, conf);

    char* automfile = 0;
    bool use_vertices = true;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--automs") == 0) {
            automfile = argv[i + 1];
        }
        if (strcmp(argv[i], "--faces") == 0) {
            use_vertices = false;
        }
    }

    if (automfile != 0) {
        FILE* file = fopen(automfile, "r");

        int v1, v2, u1, u2;

        while (fscanf(file, "%d %d %d %d", &v1, &v2, &u1, &u2) > 0) {
            if (use_vertices) {
                manager->addAutomV2V(v1, v2, u1, u2);
            } else {
                manager->addAutomF2F(v1, v2, u1, u2);
            }
        }

        fclose(file);
    } else {
        printf("Need an automorphism file!\n");
        return 1;
    }
    manager->createShapeConf();

    manager->importArguments(argc, argv);

    fflush(stdout);

    while (manager->readJob(stdin) >= 0) {
        manager->doSearch();

        manager->clear();
    }

    delete manager;
    delete conf;
    delete grid;

    return 0;
}