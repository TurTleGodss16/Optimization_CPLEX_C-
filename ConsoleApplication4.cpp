#include<ilcplex/ilocplex.h>
ILOSTLBEGIN
IloInt n;
void define_data(IloEnv env) {
	n = 5;
}
int main(int, char**) {
	IloEnv env;
	try {
		define_data(env);
		IloModel model(env);
		IloArray<IloIntVarArray> x(env, n);
		//IloArray<IloNumVarArray> x(env, n);
		IloNumVarArray u(env, n);
		IloRangeArray ctr1(env, n);
		IloRangeArray ctr2(env, n);
		IloArray<IloRangeArray> ctr3(env, n);
		u[0] = IloNumVar(env, 1, 1);
		for (int i = 1; i < n; i++)
		{
			u[i] = IloNumVar(env, 2, n);
		}
		for (int i = 0; i < n; i++)
		{
			x[i] = IloIntVarArray(env, n, 0, 1);
			//x[i] = IloNumVarArray(env, n);
			for (int j = 0; j < n; j++)
			{
				x[i][j] = IloIntVar(env, 0, 1);
				//x[i][j] = IloNumVar(env, 0, 1);
			}
		}
		IloArray<IloNumVarArray> c(env, n);
		for (int i = 0; i < n; i++)
		{
			c[i] = IloNumVarArray(env, n);
			for (int j = 0; j < n; j++)
			{
				c[i][j] = IloNumVar(env, 1, 3, ILOINT);
			}
		}
		//Object
		IloExpr obj(env);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				obj += c[i][j] * x[i][j];
			}
		}
		model.add(IloMinimize(env, obj));
		//constr1 sum x[i][j] = 1
		IloExpr expr1(env);
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				expr1 += x[i][j];
			}
			ctr1[i] = IloRange(env, 1, expr1, 1);
			expr1.clear();
		}
		model.add(ctr1);
		//constr2 sum x[j][i] = 1
		IloExpr expr2(env);
		/*for (int j = 0; j < n; ++j)
		{
			for (int i = 0; i < n; ++i)
			{
				expr2 += x[j][i];
			}
			outb[j] = IloRange(env, 1, expr2, 1);
			expr2.clear();
		}*/
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				expr2 += x[i][j];
			}
			ctr2[i] = IloRange(env, 1, expr2, 1);
			expr2.clear();
		}
		model.add(ctr2);
		//constr3 u[i]-u[j]+n*x[i][j] <= n-1;
		IloExpr expr3(env);
		ctr3[0] = IloRangeArray(env);
		for (int i = 0; i < n; i++)
		{
			ctr3[i] = IloRangeArray(env, n);
			for (int j = 0; j < n; j++)
			{
				expr3 = u[i] - u[j] + n * x[i][j];
				ctr3[i][j] = IloRange(env, -IloInfinity, expr3, n - 1);
				expr3.clear();
			}
			model.add(ctr3[i]);
		}
		// object
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
			for (int j = 0; j < n; j++)
			{
				env.out() << cplex.getValue(x[i][j]) << " ";
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