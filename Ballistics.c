#define _CRT_SECURE_NO_WARNINGS /**To avoid some of the warnings in Visual Studio**/
#define _USE_MATH_DEFINES /*To use the M_PI from math.h*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*For displaying the trajectory*/
#define WIDTH 80
#define HEIGHT 25

typedef struct {
    double target; /*The distance of the target, we will ask the user to enter it*/
    double wind; /*The speed of wind, we will ask the user to enter it*/
    double v0; /**The starting velocity, we will ask the user to enter it**/
    double m; /**The mass of the projectile, we will ask the user to enter it**/
    double A; /**The cross sectional area of the projectile, we will ask the user to enter it**/
} starting_data; /*The only purpose is to make utilizing this data in functions easier*/

typedef struct {
    int x;
    int y;
} trajectory_point; /*A point in the trajectory will have this type*/

/**A function that asks the user whether or not he wants to commit (like when he wants to exit or launch the calculations)**/
int sure(void) 
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

/*A function that will be used to read in the data from the user necessary for calculations*/
double read_data(void) 
{
    double data = 0;
    scanf("%lf", &data);
    return data;
}

/*We will use this function to check whether or not the entered value is positive, and if it is not, we prompt the user
to enter another value until the user enters a positive one. That positive value then gets returned.*/
double is_positive()
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

/*A function used for creating the grid and initializing it with empty spaces*/
char **grid_create()
{
    char** grid = (char**)malloc(HEIGHT * sizeof(char*));
    if (grid == NULL)
    {
        printf("Error! Insufficient memory!\n");
        return;
    }
    else
    {
        for (int i = 0; i < HEIGHT; i++)
        {
            grid[i] = (char*)malloc(WIDTH * sizeof(char));
            if (grid[i] == NULL) {
                printf("Error! Insufficient memory!\n");
                for (int j = 0; j < i; j++)
                {
                    free(grid[j]);
                }
                free(grid);
                return;
            }
        }
    }
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            grid[i][j] = ' ';
        }
    }
    return grid;
}

/*A function for displaying a trajectory*/
void display_trajectory(char **grid) 
{
    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}

/*A function for writing out the necessary data into a txt file - it will append to the end of the file or create a new one*/
void file_write(char **grid, double angle1, double angle2, starting_data start_data, double time) 
{
    FILE* fp;
    int status;
    fp = fopen("Ballistics .txt", "a"); 
    if (fp == NULL) 
    {
        printf("Error! Failed to create file!\n");
        return;
    }
    fprintf(fp, "The angle(s): %f", angle1);
    if (angle2 != 0)
        fprintf(fp, ", %f", angle2);
    fprintf(fp, "\nThe time of flight: %f s", time);
    fprintf(fp, "\nThe starting data:\nDistance to target: %f m\nSpeed of wind: %f m/s\nMass of projectile: %f kg\nCross-sectional area: %f m^2\nMuzzle velocity: %f m/s\n", start_data.target, start_data.wind, start_data.m, start_data.A, start_data.v0);
    fprintf(fp, "The trajectory/trajectories:\n");
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            fprintf(fp, "%c", grid[i][j]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n\n");
    status = fclose(fp); 
    if (status != 0)
    {
        printf("Error! Failed to close file!\n");
        return;
    }

}

/*This large function has three main purposes: Firstly, it calculates the angle for the given distance using physics, then it fills up the grid for printing,
and finally it updates the "found" variable, telling us the relation between the current shot,
and the previous one, so that we can identify whether it's the first angle, the second, or the only one*/
void distance(double i, starting_data start_data, double **angle, double *min, int *found, double *prev_diff, double *prev_dist, double *h_max, double *dist_max, int print, char **grid, double *time)
{
    double rho = 1.293; /**The density of air, required for the calculations**/
    double Cd = 0.295; /**The drag coefficient of a sphere, required for the calculations**/
    double g = 9.81; /**Gravitational acceleration, required for the calculations**/
    double vterminal = sqrt(start_data.m * g / (rho * Cd * start_data.A * 0.5)); /*The terminal velocity while falling, required for the calculations*/
    double dt = 0.0001; /*A "delta t", timestep which we will use for the numerical calculations. 0.001 is sufficiently small.*/
    trajectory_point trajectory;
    double drag, h, htop, hterm, ttop, tland, vx, vy, acc, displacement, difference, t;
    h = 0, htop = 0, hterm = 0, ttop = 0, tland = 0, vx = start_data.v0 * cos(i * M_PI / 180), vy = start_data.v0 * sin(i * M_PI / 180), displacement = 0, difference = 0, t = 0;
    while (tland == 0)
    {   
        if (h < 0) /*If during falling we encounter a negative height, we have touched the ground*/
            tland = t;
        t += dt;
        if (i == 0)/*Launching at 0 degrees means we land basically instantly*/
            tland = t;
        /*Calculating the velocity along the y axis*/
        if (vy > 0 && ttop == 0) /*This runs until the projectile gets slowed down below 0 - at the top of the trajectory*/
        {
            drag = rho * Cd * start_data.A * 0.5 * pow(vy, 2);
            acc = -1 * (g + drag / start_data.m); /*The acceleration is calculated from the sum of the gravitational acc. and the acc. from the drag force*/
            vy += acc * dt;
            h += vy * dt;
        }
        else if (vy <= 0 && ttop == 0) /*If this is true, we have reached the top for the first time during a flight*/
        {
            ttop = t;
            htop = h;
            if(print!=1)/*When we are printing the graph we don't want to modify this*/
                *h_max = h;
            vy = 0;
        }
        else if (ttop != 0 && hterm == 0) /*If this is true, we have reached the top before, but we haven't reached terminal velocity yet*/
        {
            drag = rho * Cd * start_data.A * 0.5 * pow(vy, 2);
            acc = g - (drag / start_data.m);
            vy += acc * dt;
            h -= vy * dt;
            if (acc < 0.000001)/*If we reach an acceleration of 0.000001, we are safe to say that we have reached a point where further acceleration is negligible - from there
                               the change in speed is so little, that we can ignore that (we would almost never reach 0 due to how velocity and drag affect each other)*/
            {
                hterm = h;
            }
        }
        else if (hterm != 0)/*We will use this after we reach terminal velocity, as then we no longer accelerate but fall with the constant speed of terminal velocity*/
        {
            h -= vterminal * dt;
        }
        /*Velocity along the x axis*/
        drag = rho * Cd * start_data.A * 0.5 * pow(vx - start_data.wind, 2);/*We need to subtract the wind, because
                                                    the user enters a negative value in case of headwind,
                                                    and a positive in the case of tailwind. However if there is a headwind,
                                                    our relative velocity to the wind adds up, and with a tailwind it gets subtracted from the velocity of the wind*/
        acc = -1 * drag / start_data.m;
        vx += acc * dt;
        displacement += vx * dt;
        if (print != 1)/*When we are printing the graph we don't want to modify this*/
            if (displacement > *dist_max)
                *dist_max = displacement;
        /*We only want to fill our array at certain calls of this function, but when that happens, this part deals with it*/
        if (print == 1)
        {   /*For displaying we scale down the trajectory with the maximal distance and height to fit into the width and length of display*/
            trajectory.x = (int)(displacement / *dist_max * WIDTH);
            trajectory.y = HEIGHT - (int)(h / *h_max * HEIGHT);
            if (trajectory.y == HEIGHT) /*Dealing with projectiles flying very low - could happen with low angles*/
                trajectory.y = HEIGHT - 1;
            if (trajectory.x >= 0 && trajectory.x < WIDTH && trajectory.y >= 0 && trajectory.y < HEIGHT)
                grid[trajectory.y][trajectory.x] = '*';
        }
    }
    *time = t;
    /*These will be used to calculate wether or not we find an optimal angle, and the "type" of the shot: is it the only one, or is there another?*/
    difference = start_data.target - displacement;
    if (fabs(difference) < *min )
    {
        *min = fabs(difference);
    }
    /*The first part will be true if we "overshot" the target: the previous was closer to us, but the current is further than the target
    The second part will be true if we were beyond the target with the previous shot, and the current shot is closer than the target*/
    if (((difference <= 0 && *prev_diff >= 0) && *prev_dist > 0) || (difference >= 0 && *prev_diff <= 0))
    {
        *found = 1;
        **angle = i;
    }
    /*This part will be true if we reached the maximum distance, as both the current and previous shots are closer than the target*/
    else if ((fabs(displacement) <= fabs(*prev_dist) && (difference >= 0 && *prev_diff >= 0)))
    {
        *found = 2;
        **angle = i;
    }
    /*This will be true if either we reach exactly the target, or our difference from the target is less than 1 milimeters - only gets used in the more precise calculations (as until then
    "found" will either be 0, 1 or 2 due to the else-if structure*/
    else if (displacement == start_data.target || fabs(displacement-start_data.target) < 0.001)
    {
        *found = 3;
        **angle = i;
    }
    *prev_diff = difference;
    *prev_dist = displacement;
}

/*Our other large function, that uses the previous distance calculator function to go through the angles, then calculate them even more precisely. 
The printing functions will also be called here*/
void calculations(starting_data start_data, double* angle1, double* angle2)
{
    /*This will store our grid that we plan on printing. We check whether we have enough memory for it*/
    char** grid = grid_create();
    double min = start_data.target, prev_diff = start_data.target, prev_dist = 0, delta = 0.1, first_dist = 0, h_max1 = 0, h_max2 = 0, h_max = 0, dist_max1 = 0, dist_max2 = 0, dist_max = 0, time=0;
    int main_found = 0, aux_found = 0, counter=0; /*We need two different variables to signal whether the target has been found, and whether it's one or two angles, plus we need a counter*/
    *angle1 = 0, * angle2 = 0;
    /*Here we find the first angle*/
    for (int i = 0; main_found == 0; i += 1)
    {
        distance(i, start_data, &angle1, &min, &main_found, &prev_diff, &prev_dist, &h_max1, &dist_max1, 0, grid, &time);
    }
    /*Now we calculate it to be more precise*/
    *angle1 -= 1, delta = 0.1;
    for (int i = 0; i < 10 && aux_found != 3; i++)
    {
        distance(*angle1, start_data, &angle1, &min, &aux_found, &prev_diff, &prev_dist, &h_max1, &dist_max1, 0, grid, &time);
        aux_found = 0, dist_max1 = 0, counter = 0;
        while (aux_found == 0 && counter < 11)
        {
            *angle1 += delta;
            distance(*angle1, start_data, &angle1, &min, &aux_found, &prev_diff, &prev_dist, &h_max1, &dist_max1, 0, grid, &time);
        }
        *angle1 -= delta;
        delta /= 10;
    }
    first_dist = prev_dist;
    if (first_dist < start_data.target && main_found == 2)    /*Checking whether this angle with the data is enough or not*/
    {
        printf("Unable to reach target, please change some variables!\nCurrently the furthest we can shoot: %f\n", first_dist);
        return;
    }
    else if (main_found == 2)     /*There is only one angle, but we can reach the target*/
    {
        printf("Only one angle found, %f, with the maximum distance of %f\n", *angle1, first_dist);
        distance(*angle1, start_data, &angle1, &min, &aux_found, &prev_diff, &prev_dist, &h_max1, &dist_max1, 1, grid, &time);
        display_trajectory(grid);
    }
    else /*There are two angles, and this is the first one*/
    {
        printf("The first angle is %f, ", *angle1);
    }
    if (main_found == 1) /*Calculating the second angle if it exists*/
    {
        min = start_data.target, aux_found = 0, prev_dist = -1;
        for (int i = (int)*angle1 + 1; aux_found == 0; i += 1)
            distance(i, start_data, &angle2, &min, &aux_found, &prev_diff, &prev_dist, &h_max2, &dist_max2, 0, grid, &time);
        /*Precision calculations*/
        *angle2 -= 1, delta = 0.1;
        for (int i = 0; i < 10 && aux_found != 3; i++)
        {
            distance(*angle2, start_data, &angle2, &min, &aux_found, &prev_diff, &prev_dist, &h_max2, &dist_max2, 0, grid, &time);
            aux_found = 0, dist_max2 = 0, counter = 0;
            while (aux_found == 0 && counter < 11)
            {
                *angle2 += delta;
                distance(*angle2, start_data, &angle2, &min, &aux_found, &prev_diff, &prev_dist, &h_max2, &dist_max2, 0, grid, &time);
            }
            *angle2 -= delta;
            delta /= 10;
        }
        printf("the second is %f\n", *angle2);
    }
    /*Printing the trajectories to the main screen*/
    h_max = fmax(h_max1, h_max2);
    dist_max = fmax(dist_max1, dist_max2);
    if(main_found == 1)
        distance(*angle2, start_data, &angle2, &min, &aux_found, &prev_diff, &prev_dist, &h_max, &dist_max, 1, grid, &time);
    distance(*angle1, start_data, &angle1, &min, &aux_found, &prev_diff, &prev_dist, &h_max, &dist_max, 1, grid, &time);
    display_trajectory(grid);
    file_write(grid, *angle1, *angle2, start_data, time);
    /*Freeing our dynamic 2D array when it's no longer needed*/
    for (int i = 0; i < HEIGHT; i++) 
    {
        free(grid[i]);
    }
    free(grid);
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