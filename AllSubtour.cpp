#include<iostream>
#include<vector>
#include<queue>
#include<ilcplex/ilocplex.h>
#define eps 1e-6
using namespace std;
/*IloInt visit(IloInt v, IloNumArray2 sol, IloNumArray seen, IloNum tol) {
    seen[v] = 1;
    IloInt n = sol.getSize();
    IloInt i;
    IloInt ret = 1;
    for (i = 0; i < v; ++i) {
        if (sol[v][i] > tol && !seen[i]) {
            ret += visit(i, sol, seen, tol);
        }
    }
    for (i = v + 1; i < n; ++i) {
        if (sol[i][v] > tol && !seen[i]) {
            ret += visit(i, sol, seen, tol);
        }
    }
    return ret;
}

IloInt checkComponent(IloNumArray2 sol, IloNumArray seen, IloNum tol) {
    IloInt j, n = sol.getSize();
    IloInt length = 0;
    IloInt current = 0;
    seen.clear();
    seen.add(n, 0.0);
    length = visit(0, sol, seen, tol);
    return length;
}
*/

class FacilityCallback : public IloCplex::Callback::Function {
private:
	FacilityCallback() {}
	FacilityCallback(const FacilityCallback& tocopy);
	IloArray<IloNumVarArray> x;

public:
	FacilityCallback(IloArray<IloNumVarArray>& _x) :
		x(_x)
	{}
	inline void linearCut(const IloCplex::Callback::Context& context) const
	{
		int n = x.getSize();
		vector <vector<double> > d(n);
		if (context.inRelaxation())
		{
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					d[i].push_back(context.getRelaxationPoint(x[i][j]));
		}
		else
		{
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					d[i].push_back(context.getCandidatePoint(x[i][j]));
		}
		vector<int> visit(n);//bfs
		fill(visit.begin(), visit.end(), 0);
		vector<IloRange> subtourAdd;//constraint subtour
		for (int i = 0; i < n; ++i)
			if (!visit[i])
			{
				vector<int> vt;
				queue <int> q;
				visit[i] = 1;
				q.push(i);
				while (!q.empty())
				{
					int u = q.front();
					q.pop();
					vt.push_back(u);
					for (int v = 0; v < n; ++v)
						if (!visit[v] && d[u][v] > eps)
						{
							visit[v] = 1;
							q.push(v);
						}
				}
				if (vt.size() == n || vt.size() == 1) break;
				IloExpr sum(context.getEnv());
				for (int u : vt)
					for (int v : vt) sum += x[u][v];
				subtourAdd.push_back(sum <= (int)vt.size() - 1);
				sum.end();
			}
		if (context.inRelaxation())
			for (auto r : subtourAdd)
				context.addUserCut(r, IloCplex::UseCutPurge, IloFalse);
		else
			for (auto r : subtourAdd)
				context.rejectCandidate(r);
	}
	void invoke(const IloCplex::Callback::Context& context);
};

void FacilityCallback::invoke(const IloCplex::Callback::Context& context)
{
	linearCut(context);
}

int main(int argc, char **argv)
{
	IloEnv env;
	try
	{
		ifstream ss3("ss3.txt");
		clock_t start = clock();
		IloInt n;
		ss3 >> n;
		IloNumArray2 d(env, n);
		for (int i = 0; i < n; i++)
		{
			d[i] = IloNumArray(env, n);
			for (int j = 0; j < n; j++)
			{
				ss3 >> d[i][j];
				//cout << d[i][j] << " ";
			}
			//cout << "\n";
		}
		IloArray<IloNumVarArray> x(env, n);
		IloModel model(env);
		for (int i = 0; i < n; ++i)
		{
			x[i] = IloNumVarArray(env, n, 0, 1, ILOINT);
			model.add(x[i]);
		}
		{
			IloExpr sum(env);
			for (int i = 0; i < n; ++i)
				for (int j = 0; j < n; ++j)
					sum += d[i][j] * x[i][j];
			model.add(IloMinimize(env, sum));
			sum.end();
		}
		for (int i = 0; i < n; ++i)
			model.add(x[i][i] == 0);
		for (int i = 0; i < n; ++i)
		{
			IloExpr sum(env);
			for (int j = 0; j < n; ++j)
				sum += x[i][j];
			model.add(sum == 1);
			sum.end();
		}
		for (int i = 0; i < n; ++i)
		{
			IloExpr sum(env);
			for (int j = 0; j < n; ++j)
				sum += x[j][i];
			model.add(sum == 1);
			sum.end();
		}
		IloCplex cplex(model);
		FacilityCallback cb(x);
		cplex.use(&cb, IloCplex::Callback::Context::Id::Candidate | IloCplex::Callback::Context::Id::Relaxation);
		//cplex.solve();
		cplex.setParam(IloCplex::PreInd, IloFalse);
		if (cplex.solve())
			env.out() << "Optimal tour length "
			<< cplex.getObjValue() << endl;
		/*for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				env.out() << cplex.getValue(x[i][j]) << " ";
			}
			cout << "\n";
		}*/

	}
	catch (const IloException & e) {
		cerr << "Exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught!" << endl;
	}

	env.end();
	return 0;
}
