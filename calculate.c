#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct _mean {
    double append;
    double find;
} mean_time;

double *compute_SD(FILE *fp, mean_time mean)
{
    char append[50], find[50];
    double append_t, find_t;
    double *SD = malloc(sizeof(double)*2);
    for(int i = 0; i < 100; i++) {
        fscanf(fp, "%s %s %lf %lf\n", append, find, &append_t, &find_t);
        SD[0] += (append_t - mean.append) * (append_t - mean.append);
        SD[1] += (find_t -mean.find) * (find_t - mean.find);
    }
    SD[0] = SD[0] / 100;
    SD[0] = sqrt(SD[0]);
    SD[1] = SD[1] / 100;
    SD[1] = sqrt(SD[1]);
    return SD;
}

double del_out_of_range(FILE *fp, double SD, double mean, int sel)
{
    char append[50], find[50];
    double append_t, find_t;
    double low_end, up_end; // lower/upper endpoint
    double number = 0, sum = 0, time = 0.0;
    fseek(fp, 0, SEEK_SET);
    low_end = mean - 1.96 * SD;
    up_end = mean + 1.96 * SD;

    for(int i = 0; i < 100 ; i++) {
        fscanf(fp, "%s %s %lf %lf\n", append, find, &append_t, &find_t);
        time = (sel) ? append_t : find_t;
        if(time >= low_end && time <= up_end) {
            sum += time;
            number++;
        }
    }
    return sum/number;
}

mean_time confidence_interval(FILE *fp, double sum_a, double sum_f)
{
    double *SD;//standard deviation 0:append 1:find
    mean_time mean;
    mean.append = sum_a/100.0;
    mean.find = sum_f/100.0;
    SD = compute_SD(fp, mean);
    mean.append = del_out_of_range(fp, SD[0], mean.append, 1);
    mean.find = del_out_of_range(fp, SD[1], mean.find, 0);
    return mean;
}

int main(void)
{
    FILE *fp = fopen("orig.txt", "r");
    FILE *output = fopen("output.txt", "w");
    if (!fp) {
        printf("ERROR opening input file orig.txt\n");
        exit(0);
    }
    char append[50], find[50];
    double orig_sum_a = 0.0, orig_sum_f = 0.0, orig_a, orig_f;
    mean_time orig_mean;
    for (int i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find,&orig_a, &orig_f);
        orig_sum_a += orig_a;
        orig_sum_f += orig_f;
    }
    fseek(fp, 0, SEEK_SET);
    orig_mean = confidence_interval(fp, orig_sum_a, orig_sum_f);
    fclose(fp);

    fp = fopen("opt.txt", "r");
    if (!fp) {
        fp = fopen("orig.txt", "r");
        if (!fp) {
            printf("ERROR opening input file opt.txt\n");
            exit(0);
        }
    }
    double opt_sum_a = 0.0, opt_sum_f = 0.0, opt_a, opt_f;
    mean_time opt_mean;
    for (int i = 0; i < 100; i++) {
        if (feof(fp)) {
            printf("ERROR: You need 100 datum instead of %d\n", i);
            printf("run 'make run' longer to get enough information\n\n");
            exit(0);
        }
        fscanf(fp, "%s %s %lf %lf\n", append, find,&opt_a, &opt_f);
        opt_sum_a += opt_a;
        opt_sum_f += opt_f;
    }
    fseek(fp, 0, SEEK_SET);
    opt_mean = confidence_interval(fp, opt_sum_a, opt_sum_f);
    fprintf(output, "append() %lf %lf\n", orig_mean.append ,
            opt_mean.append);
    fprintf(output, "findName() %lf %lf", orig_mean.find ,
            opt_mean.find);
    fclose(output);
    fclose(fp);
    return 0;
}
