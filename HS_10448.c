#include<stdio.h>

int main()
{
	int N = 0;
	int i, j, k, l = 0;
	int Num[50];
	int flag;
	
	scanf("%d", &N);
	for (i = 0; i < N; i++)
	{
		scanf("%d", &Num[i]);
	}

	for (i = 0; i < N; i++)
	{
		flag = 0;
		for (j = 1; j < 45; j++)
		{
			for (k = 0; k < 45; k++)
			{

				for (l = 0; l < 45; l++)
				{
					if (Num[i] == (j*(j + 1) / 2) + ((j + k)*(j + k + 1) / 2) + ((j + k + l)*(j + k + l + 1) / 2))
					{
						flag = 1;
					}	

				}
			}
		}
		if (flag == 1) {

			printf("1");
		}
		else
			printf("0");
		if (i != N - 1)
			printf("\n");
	}
	return 0;
}