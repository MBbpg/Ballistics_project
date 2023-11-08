#define _CRT_SECURE_NO_WARNINGS /**To avoid warnings in Visual Studio**/
#define _USE_MATH_DEFINES /*To use the M_PI from math.h*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    double target; /*The distance of the target, we will ask the user to enter it*/
    double wind; /*The speed of wind, we will ask the user to enter it*/
    double v0; /**The starting velocity, we will ask the user to enter it**/
    double m; /**The mass of the projectile, we will ask the user to enter it**/
    double A; /**The cross sectional area of the projectile, we will ask the user to enter it**/
} starting_data; /*The only purpose is to make utilizing this data in functions easier*/

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



void distance(double i, starting_data start_data, double **angle, double *min, int *found, double *prev_diff, double *prev_dist)
{
    double rho = 1.293; /**The density of air, required for the calculations**/
    double Cd = 0.295; /**The drag coefficient of a sphere, required for the calculations**/
    double g = 9.81; /**Gravitational acceleration, required for the calculations**/
    double vterminal = sqrt(start_data.m * g / (rho * Cd * start_data.A * 0.5)); /*The terminal velocity while falling, required for the calculations*/
    double dt = 0.0001; /*A "delta t", timestep which we will use for the numerical calculations. 0.001 is sufficiently small.*/
    double drag, h1, h2, htop, hterm, ttop, tland, vx, vy, acc, displacement, difference, t;
    h1 = 0, h2 = 0, htop = 0, hterm = 0, ttop = 0, tland = 0, vx = start_data.v0 * cos(i * M_PI / 180), vy = start_data.v0 * sin(i * M_PI / 180), displacement = 0, difference = 0, t = 0;
    while (tland == 0)
    {
        if (h2 < 0) /*If during falling we encounter a negative height, we have touched the ground*/
            tland = t;
        t += dt;
        if (i == 0)/*Launching at 0 degrees means we land basically instantly*/
            tland = t;
        if (vy > 0 && ttop == 0) /*This runs until the projectile gets slowed down below 0*/
        {
            drag = rho * Cd * start_data.A * 0.5 * pow(vy, 2);
            acc = -1 * (g + drag / start_data.m); /*The acceleration is calculated from the sum of the gravitational acc. and the acc. from the drag force*/
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
            drag = rho * Cd * start_data.A * 0.5 * pow(vy, 2);
            acc = g - (drag / start_data.m);
            vy += acc * dt;
            h2 -= vy * dt;
            if (acc < 0.000001)/*If we reach an acceleration of 0.000001, we are safe to say that we have reached a point where further acceleration is negligible - from there
                               the change in speed is so little, that we can ignore that*/
            {
                hterm = h2;
            }
        }
        else if (hterm != 0)/*We will use this after we reach terminal velocity, as then we no longer accelerate but fall with the constant speed of terminal velocity*/
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
            drag = rho * Cd * start_data.A * 0.5 * pow(vx - start_data.wind, 2);/*We need to subtract the wind, because
                                                        the user enters a negative value in case of headwind,
                                                        and a positive in the case of tailwind. However if there is a headwind,
                                                        our relative velocity to the wind adds up, and with a tailwind it gets subtracted from the velocity of the wind*/
            acc = -1 * drag / start_data.m;
            vx += acc * dt;
            displacement += vx * dt;
        }
    }
    printf("%f: %f\n", i, displacement);
    difference = start_data.target - displacement;
    if (fabs(difference) < *min )
    {
        *min = fabs(difference);
        **angle = i;
        /*if (displacement < target)
            **angle1 = -1 * (**angle1);*/
    }
    /*The first part will be true if we "overshot" the target: the previous was closer to us, but the current is further than the target
    The second part will be true if we were beyond the target with the previous shot, and the current shot is closer than the target
    The third (else if) part will be true if we reached the maximum distance, and both the current and previous shots are closer than the target */
    if ((difference <= 0 && *prev_diff >= 0) || (difference >= 0 && *prev_diff <= 0))
        *found = 1;
    else if ((fabs(displacement) <= fabs(*prev_dist) && (difference >= 0 && *prev_diff >= 0)))
        *found = 2;
    else if (displacement == start_data.target)
        *found = 3;
    *prev_diff = difference;
    *prev_dist = displacement;

}


void calculations(starting_data start_data, double *angle1, double *angle2)
{
    double rho = 1.293; /**The density of air, required for the calculations**/
    double Cd = 0.295; /**The drag coefficient of a sphere, required for the calculations**/
    double g = 9.81; /**Gravitational acceleration, required for the calculations**/
    double vterminal = sqrt(start_data.m * g / (rho * Cd * start_data.A * 0.5)); /*The terminal velocity while falling, required for the calculations*/
    double dt = 0.0001; /*A "delta t", timestep which we will use for the numerical calculations. 0.001 is sufficiently small.*/
    double min = start_data.target, prev_diff = start_data.target, prev_dist=0, delta=0.1;
    int found = 0;
    for (int i = 1; found==0; i += 1)
    {
        distance(i, start_data, &angle1, &min, &found, &prev_diff, &prev_dist);
    }
    if (found == 1) /*If we found an angle that is not the highest angle*/
    {
        found = 0, min = start_data.target;
        for(int i=(int)fabs(*angle1)+1; found==0; i+=1)
            distance(i, start_data, &angle2, &min, &found, &prev_diff, &prev_dist);
        printf("%f, %f\n", *angle1, *angle2);

        *angle2 -= 1.1;
        for (int i = 0; i < 21; i++)
        {
            distance(*angle2, start_data, &angle2, &min, &found, &prev_diff, &prev_dist);
            found = 0;
            while (found == 0)
            {
                *angle2 += delta;
                distance(*angle2, start_data, &angle2, &min, &found, &prev_diff, &prev_dist);
            }
            *angle2 -= delta;
            delta /= 10;
        }
    }
    if (found == 2) /*If we did find the highest angle*/
    {
        printf("%f\n", *angle1);
    }
    *angle1 -= 1;
    for (int i = 0; i < 21; i++)
    {   
        distance(*angle1, start_data, &angle1, &min, &found, &prev_diff, &prev_dist);
        found = 0;
        while (found == 0)
        {
            *angle1 += delta;
            distance(*angle1, start_data, &angle1, &min, &found, &prev_diff, &prev_dist);
        }
        *angle1 -= delta;
        delta /= 10;
    }
    if (prev_dist < start_data.target)
        printf("Unable to reach target, please change some variables!\n");
}


int main()
{
    printf("This is a program designed to calculate the firing angle for your cannon.\nYou will be able to give certain parameters to adjust the firing process to your needs.\nYou may now choose how to proceed next:\n");
    printf("(You will be returned to this menu after your choices, unless you choose to exit or launch the program.)\n");
    int choice, end = 1; /**The choice of the user and the variable that keeps the algorithm running until the user decides to end
    the program, or run it**/
    starting_data start_data = { 0, 0, 0, 0, 0 };
    double angle1 = 0, angle2 = 0; /**The two angles for the calculation **/

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
            start_data.target = is_positive();
            break;
        case 2: printf("Please enter the speed of wind in m/s!\nEnter 0 if there is no wind, and enter a negative number to indicate headwind!\n");
            start_data.wind = read_data();
            break;
        case 3: printf("Please enter the mass of the projectile in kg!\n");
            start_data.m = is_positive();
            break;
        case 4: printf("Please enter the area of the projectile in m^2!\n");
            start_data.A = is_positive();
            break;
        case 5: printf("Please enter the starting velocity in m/s!\n");
            start_data.v0 = is_positive();
            break;
        case 6: /*end = sure();*/ /**Depending on the users choice, either the program ends, and the calculations begin, or it just returns to the menu**/
            if (sure() == 0)
            {
                if (start_data.target == 0)
                    printf("Please first enter the distance to the target!\n");
                if (start_data.m == 0)
                    printf("Please first enter the mass!\n");
                if (start_data.A == 0)
                    printf("Please first enter the area!\n");
                if (start_data.v0 == 0)
                    printf("Please first enter the muzzle velocity!\n");
                if(start_data.target !=0 && start_data.m!=0 && start_data.A!=0 && start_data.v0!=0)
                    calculations(start_data, &angle1, &angle2);
            }
            break;
        case 7: end = sure(); /**Depending on the users choice, either the program ends, or it just returns to the menu**/
            break;
        default: printf("Please enter a number ranging from 1 to 7\n\n"); /**If the user entered an invalid choice, we prompt him to enter a valid one**/
        }
    }
    printf("Thank you for using the program!\n");



    return 0;
}