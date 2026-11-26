/* Compact agriculture robot data logger.
 * Build: gcc -o agri agri_robot_short.c      Run: ./agri */
#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME "field_data.dat"
#define MOIST_LOW 30.0f   /* % below this -> needs irrigation */
#define MOIST_CRIT 20.0f  /* % below this -> urgent           */
#define TEMP_HIGH 32.0f   /* C  heat stress                   */
#define HEALTH_POOR 50.0f /* crop health index 0-100          */

typedef struct {
    double lat, lon;                      /* GPS coordinates */
    float moisture, temp, humidity, health;
} Reading;

/* Store one reading (appended to binary file) */
void save(Reading r) {
    FILE *f = fopen(FILE_NAME, "ab");
    if (!f) { perror("open"); return; }
    fwrite(&r, sizeof r, 1, f);
    fclose(f);
}

/* Read all stored readings; returns count */
int load(Reading *a, int max) {
    FILE *f = fopen(FILE_NAME, "rb");
    if (!f) return 0;
    int n = fread(a, sizeof(Reading), max, f);
    fclose(f);
    return n;
}

/* 0 = OK, 1 = low, 2 = medium, 3 = high priority */
int priority(Reading r) {
    if (r.moisture >= MOIST_LOW) return 0;
    int s = (r.moisture < MOIST_CRIT) ? 3 : 2;
    s += (r.temp > TEMP_HIGH) + (r.humidity < 40) + (r.health < HEALTH_POOR);
    return s >= 5 ? 3 : s >= 3 ? 2 : 1;
}

void add(void) {
    Reading r;
    printf("lat lon moisture%% temp(C) humidity%% health: ");
    if (scanf("%lf %lf %f %f %f %f", &r.lat, &r.lon, &r.moisture,
              &r.temp, &r.humidity, &r.health) == 6) {
        save(r);
        puts("Saved.");
    } else {
        puts("Invalid input.");
        while (getchar() != '\n');
    }
}

void report(void) {
    static Reading d[10000];
    const char *name[] = {"OK", "LOW", "MEDIUM", "HIGH"};
    int n = load(d, 10000), count[4] = {0};
    double m = 0, t = 0, h = 0, c = 0;

    if (!n) { puts("No data stored."); return; }
    for (int i = 0; i < n; i++) {
        m += d[i].moisture; t += d[i].temp;
        h += d[i].humidity; c += d[i].health;
        count[priority(d[i])]++;
    }
    printf("\n--- FIELD MONITORING REPORT (%d readings) ---\n", n);
    printf("Avg moisture %.1f%%  temp %.1fC  humidity %.1f%%  health %.1f\n",
           m / n, t / n, h / n, c / n);
    printf("Irrigation: HIGH=%d MEDIUM=%d LOW=%d OK=%d\n",
           count[3], count[2], count[1], count[0]);

    puts("\nAreas needing irrigation (most urgent first):");
    for (int p = 3; p >= 1; p--)
        for (int i = 0; i < n; i++)
            if (priority(d[i]) == p)
                printf("  %-6s (%.5f, %.5f) moisture=%.1f%% health=%.1f\n",
                       name[p], d[i].lat, d[i].lon,
                       d[i].moisture, d[i].health);
}

int main(void) {
    int ch;
    do {
        printf("\n1.Add reading  2.Report  0.Exit: ");
        if (scanf("%d", &ch) != 1) break;
        if (ch == 1) add();
        else if (ch == 2) report();
    } while (ch != 0);
    return 0;
}
