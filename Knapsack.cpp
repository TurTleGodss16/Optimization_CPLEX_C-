//knapsack
#include<ilcplex/ilocplex.h>
ILOSTLBEGIN
IloInt n;
IloInt m;
IloIntArray p, b;
void define_data(IloEnv env) {
	n = 6;
	m = 10;
	p = IloIntArray(env, n, 100, 600, 1200, 2400, 500, 2000);
	b = IloIntArray(env, m, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000);
//	b = IloIntArray(env, m, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8);
}
int main(int, char**) {
	IloEnv env;
	try {
		define_data(env);
		IloModel model(env);
		IloIntVarArray x(env, n, 0, 1);
		IloRangeArray ctr(env, m);
		IloArray<IloNumArray> r(env);
		for (int i = 0; i < m; i++)
		{
			r.add(IloNumArray(env));
			for (int j = 0; j < n; j++)
			{
				r[i].add(rand()%10+1);
			}
		}
		IloExpr obj(env);
		for (int i = 0; i < n; i++)
		{
			obj += p[i] * x[i];
		}
		model.add(IloMaximize(env, obj));
		obj.clear();
		IloExpr Ctr1(env);
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; ++j)
			{
				Ctr1 += r[i][j] * x[j];
			}
			ctr[i] = IloRange(env, 0, Ctr1, b[i]);
			Ctr1.clear();
			//model.add(sum <= b[i]);
		}
		model.add(ctr);
		//result
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.solve();
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
			env.out() << "No solution" << endl;
		env.out() << "Solution status: " << cplex.getStatus() << endl;
		env.out() << cplex.getObjValue() << endl;
		for (int i = 0; i < n; i++)
		{
			env.out() << cplex.getValue(x[i]) << " ";
		}
		cout << endl;
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; j++)
			{
				env.out() << r[i][j] << " ";
			}
			cout << "\n";
		}
	}
	catch (IloException & ex) {
		cerr << "Error :" << ex << endl;
	}
	catch (...) {
		cerr << "Error" << endl;
	}
	env.end();
	return 0;
}