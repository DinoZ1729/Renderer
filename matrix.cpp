#include "util.h"

class matrix
{
public:
	int nrow;
	int ncol;
	double **m;

	matrix()
	{
		nrow = 2; ncol = 2;
		m = (double**)malloc(nrow*sizeof(double*));
		for(int i=0; i<nrow; i++)
		{
			m[i] = (double*)malloc(ncol*sizeof(double));
		}
		for(int i=0; i<nrow; i++)
			for(int j=0; j<ncol; j++)
				m[i][j] = 0;
	}

	matrix(int r, int c)
	{
		nrow = r; ncol = c;
		m = (double**)malloc(nrow*sizeof(double*));   // allocate mem
		for(int i=0; i<nrow; i++)
		{
			m[i] = (double*)malloc(ncol*sizeof(double));
		}
		for(int i=0; i<nrow; i++)   // init as identity
		{ 
			for(int j=0; j<ncol; j++)
			{
				if(i == j) m[i][j] = 1;
				else m[i][j] = 0;
			}
		}
	}

	// matrix(vec v)
	// {
	// 	nrow = 1; ncol = 4;
	// 	m = (double**)malloc(nrow*sizeof(double*));   // allocate mem
	// 	for(int i=0; i<nrow; i++)
	// 	{
	// 		m[i] = (double*)malloc(ncol*sizeof(double));
	// 	}
	// 	m[0][0] = v.x;
	// 	m[0][1] = v.y;
	// 	m[0][2] = v.z;
	// 	m[0][3] = 1;
	// }

	~matrix()
	{
		// printf("destructor:\n");
		for(int i=0; i<nrow; i++)
		{
			free(m[i]);
		}
		free(m);
	}

	matrix(const matrix& that)
	{
		// printf("copy constructor\n");

		this->nrow = that.nrow;
		this->ncol = that.ncol;

		this->m = (double**)malloc(nrow*sizeof(double*));   // allocate mem
		for(int i=0; i<this->nrow; i++)
		{
			m[i] = (double*)malloc(ncol*sizeof(double));
		}

		for(int i=0; i<that.nrow; i++)
			for(int j=0; j<that.ncol; j++)
				this->m[i][j] = that.m[i][j]; 
	}

	double* operator[](int x)
	{
		return m[x];
	}

	void operator=(const matrix &that)
	{
		// printf("= operator\n");

		for(int i=0; i<nrow; i++)  // clear current m
		{
			free(m[i]);
		}
		free(m);

		this->nrow = that.nrow;
		this->ncol = that.ncol;

		this->m = (double**)malloc(nrow*sizeof(double*));   // allocate mem
		for(int i=0; i<this->nrow; i++)
		{
			m[i] = (double*)malloc(ncol*sizeof(double));
		}

		for(int i=0; i<that.nrow; i++)
			for(int j=0; j<that.ncol; j++)
				this->m[i][j] = that.m[i][j]; 
	}

	matrix operator*(const matrix& that)
	{
		matrix ans(this->nrow, that.ncol);
		for(int i=0; i<this->nrow; i++)
		{
			for(int j=0; j<that.ncol; j++)
			{
				double x = 0;
				for(int k=0; k<this->ncol; k++)
				{
					x += (this->m[i][k])*(that.m[k][j]);
				}
				ans[i][j] = x;
			}
		}
		return ans;
	}



	matrix cofactor(int r, int c) // returns cofactor of a[r][c]
	{
		matrix ans(nrow-1, ncol-1);

		int x = 0, y = 0;
		for(int i=0; i<nrow; i++)
		{
			if(i == r) continue;
			y = 0;
			for(int j=0; j<ncol; j++)
			{
				if(j == c) continue;
				ans.m[x][y] = m[i][j];
				y++;
			}
			x++;
		}
		return ans;
	}

	double det()
	{
		if(nrow != ncol)
		{
			printf("non-square matrix: (%d, %d)\n\n", nrow, ncol);
			exit(1);
		}

		if(nrow == 1) return m[0][0]; 
		if(nrow == 2) 
		{
			// printf("det 2x2:\n");
			// show();
			double ans = m[0][0]*m[1][1] - m[1][0]*m[0][1];
			return ans;
		}

		double ans = 0;
		int sign = 1;
		for(int i=0; i<nrow; i++)
		{
			matrix c(2, 2); 
			c = cofactor(i, 0);
			double v = c.det();

			ans += (m[i][0]*sign*v);

			// if(nrow == 4)
			// printf("det row:%d coef:%lf cofac:%lf tmp_ans:%lf\n", i, m[i][0], v, ans);

			sign = sign*(-1);
		}
		return ans;
	}

	void transpose()
	{
		matrix ans(ncol, nrow); // tmp ans matrix
		for(int i=0; i<nrow; i++)
		{
			for(int j=0; j<ncol; j++)
			{
				ans[j][i] = m[i][j];
			}
		}

		for(int i=0; i<nrow; i++) // free current m
			free(m[i]);
		free(m);

		this->nrow = ans.nrow;
		this->ncol = ans.ncol;
		m = (double**)malloc(nrow*sizeof(double*)); // allocate mem
		for(int i=0; i<nrow; i++)
			m[i] = (double*)malloc(ncol*sizeof(double));

		for(int i=0; i<nrow; i++)  // copy from tmp ans matrix
			for(int j=0; j<ncol; j++)
				m[i][j] = ans[i][j];
	}

	matrix adjugate()
	{
		matrix ans(nrow, ncol);
		for(int i=0; i<nrow; i++)
		{
			for(int j=0; j<ncol; j++)
			{
				matrix c = cofactor(i, j);
				int sign = ((i+j)%2)?-1:1;
				ans.m[i][j] = sign*c.det(); 
			}
		}
		// ans.show("Adjugate Transpose");
		ans.transpose();
		// ans.show("Adjugate");
		return ans;
	}

	matrix inv()
	{
		double d = det();
		if(d == 0)
		{
			printf("INV: determinant = 0\n");
			exit(1);
		}
		matrix ans = adjugate();
		for(int i=0; i<ans.nrow; i++)
			for(int j=0; j<ans.ncol; j++)
				ans[i][j] = ans[i][j]/d;
		return ans;
	}

	void show(string s = "Matrix")
	{
		printf("  %s:\n", s.c_str());
		for(int i=0; i<nrow; i++)
		{
			printf("  ");
			for(int j=0; j<ncol; j++)
			{
				show_util(m[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}
};




/*
int main()
{
	matrix c(3, 3);

	c[0][0] = 2; c[0][1] = -3; c[0][2] =  1;
	c[1][0] = 2; c[1][1] =  0; c[1][2] = -1;
	c[2][0] = 1; c[2][1] =  4; c[2][2] =  5;

	c.show();

	printf("determinant: %lf\n\n", c.det());

	matrix a = c.adjugate();
	a.show("Adjugate");
	matrix i = c.inv();
	i.show("Inv");

	c = c*i;
	c.show("Matirx * Inverse");
	return 0;
}
*/

