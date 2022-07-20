/*
* Subor: proj1.c
* Autor: 1BIA - Nikodém Babirád
* Login: xbabir01
* Projekt: Projekt c.1 = Práce s textem
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//vstupné parametre sú riadok s príkazom a upravovaný riadok textu
//z riadku príkazu vyberie Content, Replacement a oddelovací znak a ulozi ich do samostatnych premennych char
//nahradí prvý výskyt Content a nahradí ho za Replacement
//return riadok textu s nahradeným slovom
char sConRepl(char* cmd,char* textS) 
{
    char con[101]= {};
    char rep[101]= {};
    char oddelovac=cmd[1];
    char tmp[1000];
    for(int i=0; cmd[i]!='\0'; i++)
    {
        tmp[i]=cmd[i+2];
        if ( tmp[i] == '\n' || tmp[i] == '\r' )
            tmp[i] = '\0';
    }
    for(int i=0; tmp[i]!='\0'; i++)
    {
        if(tmp[i]==oddelovac)
            break;
        con[i]=tmp[i];
    }
    while(tmp[0]!=oddelovac)
    {
        for(int i=0; tmp[i]!='\0'; i++)
        {
            tmp[i]=tmp[i+1];
        }
    }
    for(int i=0; tmp[i]!='\0'; i++)
    {
        tmp[i]=tmp[i+1];
    }
    strcpy(rep,tmp);
    char *pos = strstr(textS, con);
    size_t novaDlzka = strlen(textS) - strlen(con) + strlen(rep);
    char novyText[novaDlzka +1];
    memcpy(novyText, textS, pos - textS); //skopíruje èas stringu pred nahradenim
    memcpy(novyText + (pos - textS), rep, strlen(rep)); // nakopíruje rep
    strcpy(novyText + (pos - textS) + strlen(rep), pos + strlen(con)); // skopíruje zvysok
    strcpy(textS,novyText);
    return *textS;
}

//vstupné parametre sú riadok s príkazom a upravovaný riadok textu
//zo príkazu vyberie Content a pridá ho pred aktualny riadok textu spoli so znakom New Line
//return vráti riadok textu spolu s riadkom pridaným pomocou príkazu
char iContent(char* cmd,char* textS) //prida novy riadok s ahoj ored aktualny riadok
{
    char tmp[1000];
    for(int k=0; cmd[k]!='\0'; k++)
    {
        tmp[k]=cmd[k+1];
    }
    strcat(tmp,textS);
    strcpy(textS,tmp);
    return *textS;
}

//vstupný parameter je upravovaný riadok textu
//funkcia nahradí aktuálny riadok za NULL
//return vráti prázdny string
char delete(char* textS) 
{
    strcpy(textS,"\0");
    return *textS;
}


//vstupný parameter je upravovaný riadok textu
//na aktuálnom riadku nahradí znak New Line a \r za NULL hodnotu
//return vráti upravený text bez znakov New Line a \r
char removeEOL(char* textS)
{
    for(int i=0; textS[i]!='\0'; i++)
    {
        if ( textS[i] == '\n' || textS[i] == '\r' )
            textS[i] = '\0';
    }
    return *textS;
}


//vstupné parametre sú riadok s príkazom a upravovaný riadok textu
//funkcia z príkazu vyberie Content a odstráni znaky New Line a \r z aktuálneho riadku textu
//Content sa uloží do dočastnej premennej
//na koniec aktuálneho riadku sa pridá Content
//return navráti upravený riadok textu
char aContent(char* cmd,char* textS) 
{
    char tmp[1000];
    for(int i=0; textS[i]!='\0'; i++)
    {
        if ( textS[i] == '\n' || textS[i] == '\r' )
            textS[i] = '\0';
    }
    for(int i=0; cmd[i]!='\0'; i++)
    {
        tmp[i]=cmd[i+1];
    }
    strcat(textS,tmp);
    return *textS;
}

//vstupné parametre sú riadok s príkazom a upravovaný riadok textu
//funkcia z príkazu vyberie Content a odstráni z neho znaky New Line a \r 
//Content sa uloží do dočastnej premennej
//pred aktuálny riadok textu sa pridá Content
//return navráti upravený riadok textu
char bContent(char* cmd,char* textS) 
{
    char tmp[100];
    for(int k=0; cmd[k]!='\0'; k++)
    {
        tmp[k]=cmd[k+1];
        if ( tmp[k] == '\n' || tmp[k] == '\r' )
            tmp[k] = '\0';
    }
    strcat(tmp,textS);
    strcpy(textS,tmp);
    return *textS;
}

//vstupný parameter je aktuálny riadok príkazu
//funkcia zo Stringu príkazu vyberie číslo a uloží ho do Long int X
//return navráti hodnotu vybratú zo stringu v datovom type long int
long int numFromS(char* cmd )
{
    char tmp[100];
    long int x;
    for(int k=0; cmd[k]!='\0'; k++)
    {
        tmp[k]=cmd[k+1];
        if ( tmp[k] == '\n' || tmp[k] == '\r' )
            tmp[k] = '\0';
    }
    x = strtol(tmp,NULL,10);
    return x;
}


int main(int argc, char *argv[])
{
    if(argc==2)
    {
        int max=0;
        long int z;
        long len=1001;
        long columns=100;
        long rows=101;
        char cmd[rows][columns];
        char textS[len];
        FILE *prikazy,/**text*/;
        prikazy = fopen(argv[1], "r");// z argv1
        //text = fopen("text.txt", "rw"); //vymazat
        for(int i=0; fgets(cmd[i], len, prikazy)!='\0'; i++) {}
        fgets(textS, len, stdin);//stdin
        for(long int j=0; cmd[j][0]!='\0'; j++)
        {
            switch(cmd[j][0])
            {
            case 'i' :
                iContent(cmd[j],textS);
                break;
            case 'b' :
                bContent(cmd[j],textS);
                break;
            case 'a' :
                aContent(cmd[j],textS);
                break;
            case 'r' :
                removeEOL(textS);
                break;
            case 'd' :
                if(strlen(cmd[j])>2) //ak zadáme odstránit viac riadkov
                {
                    for(int l=1; numFromS(cmd[j])<l; l++)
                    {
                        delete(textS);
                    }
                }
                else
                {
                    delete(textS);
                }
                break;
            case 'n' :
                z=numFromS(cmd[j]);
                if(z==0)
                    z=1;
                int xy=1;
                while(xy<=z) //ak zadáme vypísať viac riadkov textu
                {
                    fprintf(stdout, "%s", textS);
                    if(fgets(textS, 1001, text)!='\0') {}
                    else
                    {
                        fclose(prikazy);
                        fprintf(stderr, "%s", "Bol vypisany max pocet riadkov!\n");
                        //skonci ak vypise max riadky
                        return 0;
                    }
                    xy++;
                }
                break;
            case 'q' :
                fclose(prikazy);
                return 0;
                break;
            case 's' :
                sConRepl(cmd[j],textS);
                break;
            case 'S' :
                /*while(strcmp(before,after)!=0)
                {
                    *before=textS;
                    *after=sConRepl(cmd[j],textS);
                    printf("%s",textS);
                    if(strcmp(before,after)==0)
                        break;
                }
                for(int num=0;strcmp(before,after)!=0;num++){
                    *before=textS;
                    *after=sConRepl(cmd[j],textS);
                    printf("%s",textS);
                }*/
                break;
            case 'g' :
                z=numFromS(cmd[j]);
                for(int gi=0; cmd[gi+1][0]!='\0'; gi++)
                   max=gi;
                if(z>0&&z<max)
                    j=z-2; //prejde na riadok príkazu ak taký riadok existuje
                break;
            default:
                fprintf(stderr, "%s", "Bol zadany neznamy prikaz!\n");
                fclose(prikazy);
                return 0;
            }
        }

        fclose(prikazy);
        fclose(text);//vymazat
        return 0;
    }
    else
    {
        fprintf(stderr, "%s", "Bol zadany nespravny argument, alebo pocet argumentov!\n"); //ak nebol zadany spravny pocet argumentov
        return 0;
    }
}


