#include<ilcplex/ilocplex.h>
ILOSTLBEGIN
IloInt n;
/*void define_data(IloEnv env) {
	n = 5;
}*/
int main(int, char**) {
	IloEnv env;
	try {
		//define_data(env);
		//n = 5;
		IloModel model(env);
		ifstream ss2("ss2.txt");
		ss2 >> n;
		IloArray<IloNumArray> c(env, n);
		for (int i = 0; i < n; ++i)
		{
			c[i] = IloNumArray(env, n);
		}
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				ss2 >> c[i][j];
			}
		}
		IloArray<IloNumVarArray> x(env, n);
		//IloArray<IloNumVarArray> x(env, n);
		IloNumVarArray u(env, n);
		IloRangeArray ctr1(env, n);
		IloRangeArray ctr2(env, n);
		IloArray<IloRangeArray> ctr3(env, n);
		u[0] = IloNumVar(env, 1, 1, ILOINT);
		for (int i = 1; i < n; i++)
		{
			u[i] = IloNumVar(env, 2, n, ILOINT);
		}
		for (int i = 0; i < n; i++)
		{
			//x[i] = IloIntVarArray(env, n, 0, 1);
			x[i] = IloNumVarArray(env, n);
			for (int j = 0; j < n; j++)
			{
				//x[i][j] = IloIntVar(env, 0, 1);
				x[i][j] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
		/*IloArray<IloNumVarArray> c(env, n);
		for (int i = 0; i < n; i++)
		{
			c[i] = IloNumVarArray(env, n);
			for (int j = 0; j < n; j++)
			{
				c[i][j] = IloNumVar(env, 1, 1, ILOINT);
			}
		}*/
		//Object
		//constr1 sum x[i][j] = 1
		IloExpr obj(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				obj += x[i][j];
			}
			ctr1[i] = IloRange(env, 1, obj, 1);
			obj.clear();
		}
		model.add(ctr1);
		//constr2 sum x[j][i] = 1
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				obj += x[j][i];
			}
			ctr2[i] = IloRange(env, 1, obj, 1);
			obj.clear();
		}
		model.add(ctr2);
		//constr3 u[i]-u[j]+n*x[i][j] <= n-1;
		ctr3[0] = IloRangeArray(env);
		for (int i = 1; i < n; i++)
		{
			ctr3[i] = IloRangeArray(env, n);
			for (int j = 1; j < n; j++)
			{
				obj = u[i] - u[j] + n * x[i][j];
				ctr3[i][j] = IloRange(env, -IloInfinity, obj, n - 1);
				obj.clear();
			}
			model.add(ctr3[i]);
		}
		// object
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				obj += c[i][j] * x[i][j];
			}
		}
		model.add(IloMinimize(env, obj));
		obj.end();
		//result
		IloCplex cplex(model);
		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.solve();
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
			env.out() << "No solution" << endl;
		env.out() << "Solution status: " << cplex.getStatus() << endl;
		env.out() << cplex.getObjValue() << endl;
		//for (int i = 0; i < n; i++) env.out() << cplex.getValue(u[i]) << " ";
		//cout << "\n";
		//cout << cplex.getValue(u[0]) << "\n";
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				env.out() << cplex.getValue(x[i][j]) << " ";
			}
			cout << "\n";
		}
		cout << "\n";
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
