#include<ilcplex/ilocplex.h>
ILOSTLBEGIN
typedef IloArray<IloIntVarArray> arcs;
IloInt n;
/*void define_data(IloEnv env) {
	n = 5;
}*/
IloInt checkTr(IloNumArray2 sol, IloNumArray seen, IloNum tol) {
	IloInt n = sol.getSize();
	IloInt i, last = -1, length = 0, curr = 0;
	seen.clear();
	seen.add(n, 0.0);
	while (seen[curr] == 0)
	{
		length++;
		seen[curr] = length;
		for (i = 0; i < curr; i++)
		{
			if (i != last && sol[curr][i] >= 1.0 - tol) break;
		}
		if (i == curr)
		{
			for (i = curr + 1; i < n; i++)
			{
				if (i != last && sol[i][curr] >= 1.0 - tol) break;
			}
		}
		if (i == n) return n + 1;
		last = curr;
		curr = i;
	}

}
ILOLAZYCONSTRAINTCALLBACK2(AllSubtour,IloArray<IloIntVarArray>, x, IloNum, tol) {
	IloInt i, j, n = x.getSize();
	IloEnv env = getEnv();
	IloNumArray2 sol(env, n);
	for (i = 0; i < n; i++)
	{
		sol[i] = IloNumArray(env);
		getValues(sol[i], x[i]);
	}
	IloNumArray seen(env);
	IloInt length = checkTr(sol, seen, tol);
	if (length >= n)
	{
		seen.end();
		for (i = 0; i < n; i++)
			sol[i].end();
		sol.end();
		return;
	}
	IloExpr mod(env);
	for (i = 0; i < n; i++)
	{
			for (j = 0; j < n; ++j)
			{
				if(i != j) mod += x[i][j];
			}
	}
	add(mod <= length - 1).end();
	mod.end();
	seen.end();
	for (i = 0; i < n; i++)
		sol[i].end();
	sol.end();

	return;
}
int main(int, char**) {
	IloEnv env;
	try {
		//define_data(env);
		//n = 5;
		IloModel model(env);
		ifstream ss3("ss3.txt");
		ss3 >> n;
		IloArray<IloNumArray> c(env, n);
		for (int i = 0; i < n; ++i)
		{
			c[i] = IloNumArray(env, n);
		}
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				ss3 >> c[i][j];
			}
		}
		IloArray<IloIntVarArray> x(env, n);
		//IloArray<IloNumVarArray> x(env, n);
		IloNumVarArray u(env, n);
		IloRangeArray ctr1(env, n);
		IloRangeArray ctr2(env, n);
		//IloArray<IloRangeArray> ctr3(env, n);
		/*u[0] = IloNumVar(env, 1, 1, ILOINT);
		for (int i = 1; i < n; i++)
		{
			u[i] = IloNumVar(env, 2, n, ILOINT);
		}*/
		for (int i = 0; i < n; i++)
			x[i] = IloIntVarArray(env, n, 0, 1);
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
				if(i != j) obj += c[i][j] * x[i][j];
			}
		}
		model.add(IloMinimize(env, obj));
		obj.clear();
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if(i != j) obj += x[i][j];
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
		/*//constr3 u[i]-u[j]+n*x[i][j] <= n-1;
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
		}*///SubTour elimination

		// object
		//obj.end();
		//result
		IloCplex cplex(model);
		IloNum tol = cplex.getParam(IloCplex::EpInt);
		IloCplex::Callback sec = cplex.use(
			AllSubtour(env, x, tol));

		cplex.setOut(env.getNullStream());
		cplex.setWarning(env.getNullStream());
		cplex.solve();
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
			env.out() << "No solution" << endl;
		env.out() << "Solution status: " << cplex.getStatus() << endl;
		env.out() << cplex.getObjValue() << endl;

		IloNumArray2 sol(env, n);
		for (int i = 0; i < n; i++)
		{
			sol[i] = IloNumArray(env);
			cplex.getValues(sol[i], x[i]);
		}
		IloNumArray seen(env);
		IloInt length = checkTr(sol, seen, tol);

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				env.out() << cplex.getValue(x[i][j]) << " ";
			}
			cout << "\n";
		}
		//assert(length == n);

		sec.end();
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
