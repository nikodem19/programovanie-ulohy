/*
* Subor: proj2.c
* Autor: 1BIA - Nikodém Babirád
* Login: xbabir01
* Projekt: Projekt c.2 = Iteracní výpoèty
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//vystupne argumenty x,n
//vypocita taylor log
//return vysledok prirodzeny log z cisla x na n iteraciach
double taylor_log(double x, unsigned int n)
{
    if(x<0)
        return NAN;
    if(x==0)
        return -INFINITY;

    double vysledok=0.0;
    //double y=1.0;
    double xx;
    double odcitat=0.0;
    double tmp=1.0;
    if(x>0&&x<1)
    {
        x=1-x;
        xx=x;
        for(unsigned int i=1; i<=n; i++)
        {
            odcitat=x/i;
            x*=xx;
            vysledok-=odcitat;
        }
    }
    if(x>=1)
    {
        for(unsigned int i=1; i<=n; i++)
        {
            tmp*=(x-1)/x;
            vysledok+=tmp/i;
        }
    }
    return vysledok;
}

//vystupne argumenty x,n
//vypocita cfrac log pomocou zretazeneho zlomka
//return vysledok prirodzeny log z cisla x na n iteraciach
double cfrac_log(double x, unsigned int n)
{
    if(x<0)
        return NAN;
    if(x==0)
        return -INFINITY;
    double xUpr = (x - 1) / (x + 1);
    double xUpr_2 = xUpr * xUpr;
    double odcitat = 1.0;
    double zlomok = xUpr;
    double vysledok = zlomok;
    for(unsigned int i=n; i>0 && n!=1; i--)
    {
        odcitat += 2.0;
        zlomok *= xUpr_2;
        vysledok += zlomok / odcitat;
    }
    return 2.0 * vysledok;
}

//vystupne argumenty x,y,n,ln
//vypocita pow zo zadanych cisel
//return vysledok x na y pri n iteraciach
double pow_calc(double x, double y, unsigned int n,double ln)
{
    if(x==0)
        return 0;
    double vysledok=1.0;
    double lnh=ln;
    double fac=1.0;
    double yy=y;
    if(x>0)
    {
        for(unsigned int i=1; i<n; i++)
        {
            fac*=i;
            vysledok+=y*ln/fac;
            y*=yy;
            ln*=lnh;
        }
    }
    return vysledok;
}

//funkcia využíva vypocet pow cez vseobecnú funkciu
//pri vypocte zadáva log vypocítany z taylor_log
double taylor_pow(double x, double y, unsigned int n)
{
    return pow_calc(x,y,n,taylor_log(x,n));
}

//funkcia využíva vypocet pow cez vseobecnú funkciu
//pri vypocte zadáva log vypocítany z cfrac_log
double taylorcf_pow(double x, double y, unsigned int n)
{
    return pow_calc(x,y,n,cfrac_log(x,n));
}

int main(int argc, char * argv[])
{
    double x,y;
    int n;
    if(argc==4) //ak su zadane 4 argumenty
    {
        if (strcmp(argv[1],"--log")==0) //ak sa prvy argument rovná --log
        {
            x=atof(argv[2]);   //argumenty poukladá do premennych
            n=atof(argv[3]);
            if(n<1) //ak bol zadany neplatny pocet iteracii
            {
                fprintf(stderr, "%s", "Zadany zly pocet iteracii.\n");
                return 0;
            }
            printf("%10s(%.5g) = %.12g\n","log", x, log(x));  //vypis na 12 miest zarovnane podla znamienka =
            printf("%10s(%.5g) = %.12g\n","cfrac_log", x, cfrac_log(x,n));
            printf("%10s(%.5g) = %.12g\n","taylor_log", x, taylor_log(x,n));
        }
        else
        {
            fprintf(stderr, "%s", "Zadany nespravny argument!\n"); //skonci ak pri 4 argumentoch nie je prvy --log
            return 0;
        }
    }

    if(argc==5)  //ak je zadane 5 argumentov
    {
        if (strcmp(argv[1],"--pow")==0) //ak sa prvy argument rovná --pow
        {
            x=atof(argv[2]);  //argumenty poukladá do premennych
            y=atof(argv[3]);
            n=atof(argv[4]);
            if(n<1) //ak bol zadany neplatny pocet iteracii
            {
                fprintf(stderr, "%s", "Zadany zly pocet iteracii.\n");
                return 0;
            }
            printf("%12s(%.3g,%.2g) = %.12g\n","pow", x, y, pow(x,y));  //vypis na 12 miest zarovnane podla znamienka =
            printf("%12s(%.3g,%.2g) = %.12g\n","taylor_pow", x, y, taylor_pow(x,y,n));
            printf("%12s(%.3g,%.2g) = %.12g\n","taylorcf_pow", x, y, taylorcf_pow(x,y,n));
        }
        else
        {
            fprintf(stderr, "%s", "Zadany nespravny argument!\n"); //skonci ak pri 5 argumentoch nie je prvy --pow
            return 0;
        }
    }
    if(argc!=4 && argc!=5) //skoneí program ak neni zadaný správny pocet argumentov
    {
        fprintf(stderr, "%s", "Zadany zly pocet argumentov.\n");
        return 0;
    }
    return 0;
}
