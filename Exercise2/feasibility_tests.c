#include <math.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define U32_T unsigned int

#define NUM_TESTS (10)
#define MAX_SERVICES (4)

U32_T test_services_cnt[NUM_TESTS] = {3,3,4,3,3,3,4,3,4,4};

U32_T test_periods[NUM_TESTS][MAX_SERVICES] = 
                           {{2, 10, 15},    // example 0
                            {2, 5, 7},      // example 1
                            {2, 5, 7, 13},  // example 2
                            {3, 5, 15},     // example 3
                            {2, 4, 16},     // example 4
                            {2, 5, 10},  // example 5-1
                            {2, 5, 7, 13},  // example 6
                            {3, 5, 15},  // example 7
                            {2, 5, 7, 13},  // example 8
                            {6, 8, 12, 24}}; // example 9

U32_T test_deadlines[NUM_TESTS][MAX_SERVICES] = 
                           {{2, 10, 15},    // example 0
                            {2, 5, 7},      // example 1
                            {2, 5, 7, 13},  // example 2
                            {3, 5, 15},     // example 3
                            {2, 4, 16},     // example 4
                            {2, 5, 10},  // example 5-1
                            {2, 5, 7, 13},  // example 6
                            {3, 5, 15},  // example 7
                            {2, 5, 7, 13},  // example 8
                            {6, 8, 12, 24}}; // example 9


U32_T test_wcets[NUM_TESTS][MAX_SERVICES] = 
                         {{1, 1, 2},      // example 0
                          {1, 1, 2},      // example 1
                          {1, 1, 1, 2},   // example 2
                          {1, 2, 3},      // example 3
                          {1, 1, 4},      // example 4
                          {1, 2, 1},      // example 5-1
                          {1, 1, 1, 2},  // example 6
                          {1, 2, 4},  // example 7
                          {1, 1, 1, 2},  // example 8
                          {1, 2, 4, 6}};  // example 9

U32_T ex6_dm_period[]   = {2, 5, 7, 13};
U32_T ex6_dm_wcet[]     = {1, 1, 1, 2};
U32_T ex6_dm_deadline[] = {2, 4, 7, 20};

int rate_monotonic_least_upper_bound(U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[]);
int completion_time_feasibility(U32_T num_services, U32_T period[], U32_T wcet[], U32_T deadline[]);
int scheduling_point_feasibility(U32_T num_services, U32_T period[], U32_T wcet[], U32_T deadline[]);
int edf_dynamic_priority_feasibility(U32_T num_services, U32_T period[], U32_T wcet[], U32_T deadline[]);

// helper function to determine utilization from a set of services
float compute_utilization(U32_T num_services, U32_T period[], U32_T wcet[]);

int main(void)
{ 
    int i;
	U32_T num_services;

    for (unsigned int i = 0; i < NUM_TESTS; ++i) {
        num_services = test_services_cnt[i];
        
        printf("Ex-%d U=%4.6f (", i, compute_utilization(test_services_cnt[i], test_deadlines[i], test_wcets[i]));

        for(unsigned int si = 0; si < num_services; ++si) {
            
            printf("{S%d: C%d=%u, T%d=%u, D%d=%u}",si, si, test_wcets[i][si], si, test_periods[i][si], si, test_deadlines[i][si]);
            if (si != num_services-1) {printf(";   ");}
        }
        printf(")\n");

        if(rate_monotonic_least_upper_bound(num_services, test_periods[i], test_wcets[i], test_deadlines[i]) == TRUE)
            printf("RM LUB Test Feasibility: FEASIBLE\n");
        else
            printf("RM LUB Test Feasibility: INFEASIBLE\n");
        

        printf("Completion Test Feasibility: ");

        if(completion_time_feasibility(num_services, test_periods[i], test_wcets[i], test_deadlines[i]) == TRUE)
            printf("FEASIBLE\n");
        else
            printf("INFEASIBLE\n");

        printf("Scheduling Point Feasibility: ");

        if(scheduling_point_feasibility(num_services, test_periods[i], test_wcets[i], test_deadlines[i]) == TRUE)
            printf("FEASIBLE\n");
        else
            printf("INFEASIBLE\n");

        printf("EDF Feasibility: ");

        if(edf_dynamic_priority_feasibility(num_services, test_periods[i], test_wcets[i], test_deadlines[i]) == TRUE)
            printf("FEASIBLE\n");
        else
            printf("INFEASIBLE\n");

        printf("\n");
    }

    // Run the deadline monotonic case as well, where the period is different
    num_services = 4;
    printf("Ex-6 DM U=%4.6f (", compute_utilization(num_services,  ex6_dm_deadline, ex6_dm_wcet));
        for(unsigned int si = 0; si < num_services; ++si) {
            
            printf("{S%d: C%d=%u, T%d=%u, D%d=%u}",si, si, ex6_dm_wcet[si], si, ex6_dm_period[si], si, ex6_dm_deadline[si]);
            if (si != num_services-1) {printf(";   ");}
        }
        printf(")\n");

        printf("Completion Test Feasibility: ");

        if(completion_time_feasibility(num_services, ex6_dm_period, ex6_dm_wcet, ex6_dm_deadline) == TRUE)
            printf("FEASIBLE\n");
        else
            printf("INFEASIBLE\n");

        printf("Scheduling Point Feasibility: ");

        if(scheduling_point_feasibility(num_services, ex6_dm_period, ex6_dm_wcet, ex6_dm_deadline) == TRUE)
            printf("FEASIBLE\n");
        else
            printf("INFEASIBLE\n");

        printf("Dynamic Priority Feasibility: ");

        if(edf_dynamic_priority_feasibility(num_services, ex6_dm_period, ex6_dm_wcet, ex6_dm_deadline) == TRUE)
            printf("FEASIBLE\n");
        else
            printf("INFEASIBLE\n");

        printf("\n");
}


int completion_time_feasibility(U32_T num_services, U32_T period[], U32_T wcet[], U32_T deadline[])
{
  int i, j;
  U32_T an, anext;
  
  // assume feasible until we find otherwise
  int set_feasible=TRUE;
   
  //printf("num_services=%d\n", num_services);
  
  for (i=0; i < num_services; i++)
  {
       an=0; anext=0;
       
       for (j=0; j <= i; j++)
       {
           an+=wcet[j];
       }
       
	   //printf("i=%d, an=%d\n", i, an);

       while(1)
       {
             anext=wcet[i];
	     
             for (j=0; j < i; j++)
                 anext += ceil(((double)an)/((double)period[j]))*wcet[j];
		 
             if (anext == an)
                break;
             else
                an=anext;

			 //printf("an=%d, anext=%d\n", an, anext);
       }
       
	   //printf("an=%d, deadline[%d]=%d\n", an, i, deadline[i]);

       if (an > deadline[i])
       {
          set_feasible=FALSE;
       }
  }
  
  return set_feasible;
}


int scheduling_point_feasibility(U32_T num_services, U32_T period[], 
								 U32_T wcet[], U32_T deadline[])
{
   int rc = TRUE, i, j, k, l, status, temp;

   for (i=0; i < num_services; i++) // iterate from highest to lowest priority
   {
      status=0;

      for (k=0; k<=i; k++) 
      {
          for (l=1; l <= (floor((double)period[i]/(double)period[k])); l++)
          {
               temp=0;

               for (j=0; j<=i; j++) temp += wcet[j] * ceil((double)l*(double)period[k]/(double)period[j]);

               if (temp <= (l*period[k]))
			   {
				   status=1;
				   break;
			   }
           }
           if (status) break;
      }
      if (!status) rc=FALSE;
   }
   return rc;
}

int edf_dynamic_priority_feasibility(U32_T num_services, U32_T period[], 
								 U32_T wcet[], U32_T deadline[])
{
    int rc = TRUE, i, j, k, l, status, min_deadline;

    float summation = 0.0;
    float U = 0.0;

    // initialize the minimum deadline to the first in the list
    min_deadline = deadline[1];

    for (i=0; i < num_services; i++) // iterate from highest to lowest priority
    {
        status=0;

        if (deadline[i] < min_deadline) {
            min_deadline = deadline[i];
        }

        // pick the minimum between the deadline and the period
        float d_prime = (deadline[i] < period[i]) ? (float) deadline[i] : (float) period[i];

        summation += ((1.0 - (d_prime / ((float) period[i]))) * ((float) wcet[i]));

        // printf("culumitive summation for S%d is %f\n", i, summation);

        U += ((float)wcet[i] / (float)period[i]);
    }

    // printf("minimum deadline is %d\n", min_deadline);

    float utilization_limit = 1.0 - ((1.0 / ((float) min_deadline)) *  summation);

    // printf("U limit is %f\n", utilization_limit);
    // printf("U is %f\n", U);

    return ((U <= utilization_limit) ? TRUE : FALSE);
}

float compute_utilization(U32_T num_services, U32_T period[], U32_T wcet[]) {

    float utilization = 0.0;

    for (unsigned int idx = 0; idx < num_services; ++idx) {
        utilization += ((float)wcet[idx] / (float)period[idx]);
    }
    
    return utilization;
}

int rate_monotonic_least_upper_bound(U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[])
{
  double utility_sum=0.0, lub=0.0;
  int idx;

  //printf("for %d, utility_sum = %lf\n", numServices, utility_sum);

  // Sum the C(i) over the T(i)
  for(idx=0; idx < numServices; idx++)
  {
    utility_sum += ((double)wcet[idx] / (double)period[idx]);
    //printf("for %d, wcet=%lf, period=%lf, utility_sum = %lf\n", idx, (double)wcet[idx], (double)period[idx], utility_sum);
  }
  //printf("utility_sum = %lf\n", utility_sum);

  // Compute LUB for number of services
  lub = (double)numServices * (pow(2.0, (1.0/((double)numServices))) - 1.0);
  //printf("LUB = %lf\n", lub);

  printf("RM LUB = %lf\n",lub);

  // Compare the utilty to the bound and return feasibility
  if(utility_sum <= lub)
  {
	return TRUE;
  }
  else
  {
	return FALSE;
  }
}