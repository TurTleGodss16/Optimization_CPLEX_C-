//knapsack
#include<ilcplex/ilocplex.h>
ILOSTLBEGIN
IloInt t;
//IloInt check;
//IloIntArray p, b;
/*void define_data(IloEnv env) {
	n = 6;
	m = 10;
	p = IloIntArray(env, n, 100, 600, 1200, 2400, 500, 2000);
	b = IloIntArray(env, m, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000);
//	b = IloIntArray(env, m, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8);
}*/
int main(int, char**) {
	IloEnv env;
	try {
		//define_data(env);
		ifstream ss1("ss1.txt");
		ss1 >> t;
		while(t--)
		{
			IloModel model(env);
			IloInt n, m;
			ss1 >> n;
			ss1 >> m;
			//ss1 >> check;
			IloArray<IloNumArray> c(env, m);
			for (int i = 0; i < m; i++)
			{
				c[i] = IloNumArray(env, n);
			}
			IloNumArray p(env, n);
			IloNumArray b(env, m);
			for (int i = 0; i < n; i++) ss1 >> p[i];
			for (int i = 0; i < m; i++)
			{
				for (int j = 0; j < n; j++)
				{
					ss1 >> c[i][j];
				}
			}
			for (int i = 0; i < m; i++) ss1 >> b[i];
			/*if (check == 3800) cout << "LALALLA";
			else cout << "Error" << "\n";*/
			IloIntVarArray x(env, n, 0, 1);
			IloRangeArray ctr(env, m);
			/*	IloArray<IloNumArray> r(env);
				for (int i = 0; i < m; i++)
				{
					r.add(IloNumArray(env));
					for (int j = 0; j < n; j++)
					{
						r[i].add(rand()%10 + 1);
					}
				}*/
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
					Ctr1 += c[i][j] * x[j];
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
