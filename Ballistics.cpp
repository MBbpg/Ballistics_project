#define _CRT_SECURE_NO_WARNINGS /**To avoid warnings in Visual Studio**/
#define _USE_MATH_DEFINES /*To use the M_PI from math.h*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int sure(void) /**A function that asks the user whether or not he wants to commit (like when he wants to exit or launch the calculations)**/
{
    int choice, end = 0; /**The variable that stores the users choice, and the variable that keeps the algorithm running until
    the user inputs a valid choice**/
    while (end == 0)
    {
        printf("Are you sure you wish to proceed?\n");
        printf("  1. Yes\n  2. No\n");
        printf("Please enter the number of your choice: ");
        scanf("%d", &choice);
        printf("\n");
        switch (choice)
        {
        case 1: return 0; /**Knowing that the function returns a 0, we can use it to e.g. stop an infinite loop**/
        case 2: return 1; /**Knowing that the function returns a 1, we can adjust our loops other loops so that they keep running**/
        default: printf("Please enter either 1 or 2\n"); /**If the user entered an invalid choice, we prompt him to enter a valid one**/
        }
    }
}

double read_data(void) /*A function that will be used to read in the data from the user necessary for calculations*/
{
    double data = 0;
    scanf("%lf", &data);
    return data;
}

double is_positive() /*We will use this function to check whether or not the value is positive, and if it is not, we prompt the user
                                to enter another value until the user enters a positive one. That positive value then gets returned.*/
{   
    double data = read_data();
    while (1)
    {
        if (data <= 0)
        {
            printf("Please enter a number greater than 0!\n");
            data = read_data();
        }
        else
            break;
    }
    return data;
}

double difference(double a, double b) /*We will use this function to calculate the difference between the displacement 
                                      of the projectile with a given angle, and the target distance*/
{
    return fabs(a - b);
}

void distance(int i, double target, double wind, double v0, double m, double A, double **angle1, double **angle2, double *min1, double *min2)
{
    double rho = 1.293; /**The density of air, required for the calculations**/
    double Cd = 0.295; /**The drag coefficient of a sphere, required for the calculations**/
    double g = 9.81; /**Gravitational acceleration, required for the calculations**/
    double vterminal = sqrt(m * g / (rho * Cd * A * 0.5)); /*The terminal velocity while falling, required for the calculations*/
    double dt = 0.0001; /*A "delta t", timestep which we will use for the numerical calculations. 0.001 is sufficiently small.*/
    double drag, h1, h2, htop, hterm, ttop, tland, vx, vy, acc, displacement, t;
    h1 = 0, h2 = 0, htop = 0, hterm = 0, ttop = 0, tland = 0, vx = v0 * cos(i * M_PI / 180), vy = v0 * sin(i * M_PI / 180), displacement = 0, t = 0;
    while (tland == 0)
    {
        if (h2 < 0) /*If during falling we encounter a negative height, we have touched the ground*/
            tland = t;
        t += dt;
        if (i == 0)/*Launching at 0 degrees means we land basically instantly*/
            tland = t;
        if (vy > 0 && ttop == 0) /*This runs until the projectile gets slowed down below 0*/
        {
            drag = rho * Cd * A * 0.5 * pow(vy, 2);
            acc = -1 * (g + drag / m); /*The acceleration is calculated from the sum of the gravitational acc. and the acc. from the drag force*/
            vy += acc * dt;
            h1 += vy * dt;
        }
        else if (vy <= 0 && ttop == 0) /*If this is true, we have reached the top for the first time during a flight*/
        {
            ttop = t;
            htop = h1;
            h2 = h1;
            vy = 0;
        }
        else if (ttop != 0 && hterm == 0) /*If this is true, we have reached the top before, but we haven't reached terminal velocity yet*/
        {
            drag = rho * Cd * A * 0.5 * pow(vy, 2);
            acc = g - (drag / m);
            vy += acc * dt;
            h2 -= vy * dt;
            if (acc < 0.000001)/*If we reach an acceleration of 0.000001, we are safe to say that we have reached a point where further acceleration is negligible*/
            {
                hterm = h2;
            }
        }
        else if (hterm != 0)/*We will use this after we reach terminal velocity, as then we no longer accelerate*/
        {
            h2 -= vterminal * dt;
        }

    }
    t = 0;
    while (t <= tland)
    {
        t += dt;
        if (vx >= 0)
        {
            drag = rho * Cd * A * 0.5 * pow(vx - wind, 2);/*We need to subtract the wind, because
                                                        the user enters a negative value in case of headwind,
                                                        and a positive in the case of tailwind. However if there is a headwind,
                                                        our relative velocity to the wind adds up, and with a tailwind it gets subtracted from the velocity of the wind*/
            acc = -1 * drag / m;
            vx += acc * dt;
            displacement += vx * dt;
        }
    }
    printf("%d: %f\n", i, displacement);
    if (difference(displacement, target) < *min1)
    {
        *min1 = difference(displacement, target);
        **angle1 = i;
    }
    else if (difference(displacement, target) < *min2)
    {
        *min2 = difference(displacement, target);
        **angle2 = i;
    }
}


void calculations(double target, double wind, double v0, double m, double A, double *angle1, double *angle2)
{
    double rho = 1.293; /**The density of air, required for the calculations**/
    double Cd = 0.295; /**The drag coefficient of a sphere, required for the calculations**/
    double g = 9.81; /**Gravitational acceleration, required for the calculations**/
    double vterminal = sqrt(m * g / (rho * Cd * A * 0.5)); /*The terminal velocity while falling, required for the calculations*/
    double dt = 0.0001; /*A "delta t", timestep which we will use for the numerical calculations. 0.001 is sufficiently small.*/
    double drag, h1, h2, htop, hterm, ttop, tland, vx, vy, acc, displacement, t, min1=target, min2=target; /**/
    for (int i = 1; i <= 90; i += 1)
    {
        distance(i, target, wind, v0, m, A, &angle1, &angle2, &min1, &min2);
    }
    printf("%f, %f\n", *angle1, *angle2);
}


int main()
{
    printf("This is a program designed to calculate the firing angle for your cannon.\nYou will be able to give certain parameters to adjust the firing process to your needs.\nYou may now choose how to proceed next:\n");
    printf("(You will be returned to this menu after your choices, unless you choose to exit or launch the program.)\n");
    int choice, end = 1; /**The choice of the user and the variable that keeps the algorithm running until the user decides to end
    the program, or run it**/
    double target = 0; /*The distance of the target, we will ask the user to enter it*/
    double wind = 0; /*The speed of wind, we will ask the user to enter it*/
    double v0 = 0; /**The starting velocity, we will ask the user to enter it**/
    double m = 0; /**The mass of the projectile, we will ask the user to enter it**/
    double A = 0; /**The cross sectional area of the projectile, we will ask the user to enter it**/
    double angle1 = 0, angle2 = 0;

    /**The main menu that is being displayed**/
    while (end == 1)
    {
        printf("\nMain menu:\n  1. I would like to give information about the position of my target.\n  2. I would like to give information about the current wind.\n  3. I would like to give information about the mass of the projectile.\n  4. I would like to give information about the area of the projectile\n  5. I would like to give information about the muzzle velocity of the cannon.\n  6. I would like the program to begin the calculations.\n  7. I would like to exit the program.\n");
        printf("Please enter the number of your choice: ");
        scanf("%d", &choice);
        printf("\n");
        switch (choice)
        {
        case 1: printf("Please enter the distance of the target in meters!\n");
            target = is_positive();
            break;
        case 2: printf("Please enter the speed of wind in m/s!\nEnter 0 if there is no wind, and enter a negative number to indicate headwind!\n");
            wind = read_data();
            break;
        case 3: printf("Please enter the mass of the projectile in kg!\n");
            m = is_positive();
            break;
        case 4: printf("Please enter the area of the projectile in m^2!\n");
            A = is_positive();
            break;
        case 5: printf("Please enter the starting velocity in m/s!\n");
            v0 = is_positive();
            break;
        case 6: end = sure(); /**Depending on the users choice, either the program ends, and the calculations begin, or it just returns to the menu**/
            if (end == 0)
                calculations(target, wind, v0, m, A, &angle1, &angle2);
            break;
        case 7: end = sure(); /**Depending on the users choice, either the program ends, or it just returns to the menu**/
            break;
        default: printf("Please enter a number ranging from 1 to 7\n\n"); /**If the user entered an invalid choice, we prompt him to enter a valid one**/
        }
    }
    printf("Thank you for using the program!\n");



    return 0;
}