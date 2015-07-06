

#include "Grid.hpp"
#include "Configuration.hpp"
#include "SearchManager.hpp"
#include "fraction.hpp"

#include <stack>

namespace adage
{
namespace grids
{


class GridAutomManager : public SearchManager
{
protected:
	Grid* grid;
	Configuration* conf;
	Configuration* base_conf;
	Configuration* shape_conf;
	fraction best_density;

	int size_autom_list;
	int num_automs;
	int** f_automs;
	int** v_automs;

public:
	GridAutomManager(Grid* grid, Configuration* conf);
	virtual ~GridAutomManager();

	void addAutomF2F(int f1, int f2, int g1, int g2);
	void addAutomV2V(int v1, int v2, int u1, int u2);
	void createShapeConf();

	void clear();
	LONG_T pushNext();
	LONG_T pushTo(LONG_T child);
	int prune();
	LONG_T pop();
	int isSolution();
};


}
}