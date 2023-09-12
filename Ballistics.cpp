#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main()
{	
	printf("This is a program designed to calculate the firing angle for your cannon.\nYou will be able to give certain parameters to adjust the firing process to your needs.\nYou may now choose how to proceed next:\n");
	printf("(You will be returned to this menu after your choices, unless you choose to exit or launch the program.)\n");
	/*We need here the infinite while, until either the user exits, or decides that he has given all data and wishes to actually run the calculations.*/
	int end = 0; /*We will change it when the menu ends due to either the user launching the calculations, or due to the user exiting*/
	int choice = 0; /*This variable will be used to read in the choice of the user*/
	int yesorno = 0; /*We will use this variable when asking a yes or no question from the user*/
	int second_end = 0; /*Another variable to use for ending a while algorithm*/
	while (end < 1)
	{
		printf("  1. I would like to give information about the position of my target.\n  2. I would like to give information about the current wind.\n  3. I would like to give information about the projectile of the cannon.\n  4. I would like the program to begin the calculations.\n  5. I would like to exit the program.\n");
		printf("Please enter the number of your choice: ");
		scanf("%d", &choice);
		printf("\n");
		if (choice == 1)
		{

		}
		else if (choice == 2)
		{

		}
		else if (choice == 3)
		{

		}
		else if (choice == 4)
		{
			printf("Are you sure you would like to launch the program?\n");
			printf("  1. Yes\n  2. No\n");
			printf("Please enter the number of your choice: ");
			scanf("%d", &yesorno);
			printf("\n");
			second_end = 0;
			while (second_end < 1)
			{
				if (yesorno == 1)
				{
					end = 2;
					second_end = 1;
					printf("The calculations will begin now. You may see the results.\n");

				}
				else if (yesorno == 2)
				{
					second_end = 1;
					printf("You are redirected to the menu now.\n\n");
				}
				else
				{
					printf("Please enter a valid choice, either 1 or 2: ");
					scanf("%d", &yesorno);
					printf("\n");
				}

			}
		}
		else if (choice == 5)
		{
			printf("Are you sure you would like to end the program?\n");
			printf("  1. Yes\n  2. No\n");
			second_end = 0;
			printf("Please enter the number of your choice: ");
			scanf("%d", &yesorno);
			printf("\n");
			while (second_end < 1)
			{
				if (yesorno == 1)
				{
					end = 2;
					second_end = 1;
					printf("Thank you for trying the program! Have a great time!\n");

				}
				else if (yesorno == 2)
				{
					second_end = 1;
					printf("You are redirected to the menu now.\n\n");
				}
				else
				{
					printf("Please enter a valid choice, either 1 or 2: ");
					scanf("%d", &yesorno);
					printf("\n");
				}
			}


		}
		else
		{
			printf("Please enter a valid choice, ranging from 1 to 5.\n");
			printf("You are redirected to the menu now.\n\n");
		}
	}



	return 0;
}
