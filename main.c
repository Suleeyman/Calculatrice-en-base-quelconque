#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Un type représentant un nombre à 1 000 chiffres maximums */
typedef struct {
    char chaine[1000]; /* Stocke les caractères du nombre */
    int length;
} nombre_t;

/* Un type représentant une base allant de 0 à 96 */
typedef struct {
    int base;
    char chaineBase[95]; /* Contient les caractères de la base */
} base_t;

/* Un type pour le résultat */
typedef struct {
    char * resultat;
    int length;
} resultat_t;

/* FONCTION UTILITAIRE */

int verificationDuNombre(nombre_t, base_t);      // Verifie si un nombre possede un caractere inconnu, retourne 1 si oui, 0 sinon
int intcmp(nombre_t nombre1, nombre_t nombre2);     // Renvoie 1 si le nombre1 >= nombre2, 0 sinon
char convertir_chiffre_a_caractere(int, base_t);    // Convertis un nombre au caractere associé à sa base.
void affichage(char *, int, base_t);                // Affiche la chaine de caractere
void reverse(char *, int);                          // Retourne une chaine de caractere
void returnChiffre(nombre_t *, base_t);             // Initialise chaque case memoire par un chiffre associe a la base

/* OPERATIONS ASSOCIE A LA MULTIPLICATION ET DIVISION */
void additionnerDeuxNombres(nombre_t nombre1, resultat_t * resultat, int); // Ajoute dans le resultat la somme du nombre et d'un resultat
int multipleResultant(char *, int*, nombre_t, base_t);                 // Retorune un multiple

/* OPERATION => Chaque operation est effectuée comme en ecole primaire */
void addition(nombre_t, nombre_t, resultat_t *, base_t);
void soustraction(nombre_t, nombre_t, resultat_t *, base_t);
void multiplication(nombre_t, nombre_t, resultat_t *, base_t);
void division(nombre_t, nombre_t, resultat_t *, base_t);

int main(int argc, char * argv[])
{
    if(argc > 2) {
        printf("Pas plus de 2 arguments\n");
        return 0;
    }
    FILE * fa = fopen(argv[1], "r");
    if (fa == NULL) {
        printf("fichier introuvee\n");
        return 0;
    }

    base_t base;
    nombre_t nombre1, nombre2;
    char operation;

    /* La fonction fgets stocke une ligne entière d'un fichier */
    fgets(base.chaineBase, 95, fa);
    base.base = strlen(base.chaineBase) - 1;

    if(base.base == 1) {
        printf("La calculatrice existe que pour la base 2, jusque la base 96");
        fclose(fa);
        return 0;
    }

    for(int i = 0; i < base.base; i++) {
        if(base.chaineBase[i] < 32 || base.chaineBase[i] > 126) {
            printf("Cette calculatrice opere uniquement pour les characteres appartenant a la table ASCII; entre [32, 126].\n%c à la position %i n'est pas dans cet intervale (=%i)\n", base.chaineBase[i], i, base.chaineBase[i]);
            fclose(fa);
            return 0;
        }
    }

    printf("base: %i\n", base.base);

    operation = fgetc(fa);
    fgetc(fa); //pour le caractère de saut de ligne

    fgets(nombre1.chaine, 1000, fa);
    fgets(nombre2.chaine, 1000, fa);

    if(nombre1.chaine[0] == base.chaineBase[0] || nombre2.chaine[0] == base.chaineBase[0]) {
        printf("Ne commencez pas votre nombre par un zero\n");
        fclose(fa);
        return 0;
    }
    
    nombre1.length = strlen(nombre1.chaine) - 1; // -1 pour le caractère de saut de ligne
    nombre2.length = strlen(nombre2.chaine);
    
    if ( !verificationDuNombre(nombre1, base) ) {
        printf("Un charactere du premier nombre n'est pas un caractere de votre base\n");
        fclose(fa);
        return 0;
    }

    if ( !verificationDuNombre(nombre2, base) ) {
        printf("Un charactere du deuxieme nombre n'est pas un caractere de votre base\n");
        fclose(fa);
        return 0;
    }

    returnChiffre(&nombre1, base);
    returnChiffre(&nombre2, base);

    int maxLength = nombre1.length >= nombre2.length ? nombre1.length : nombre2.length;

    if ( operation == '+' ) {
        resultat_t additionArr;
        additionArr.resultat = malloc(maxLength); //Alloue la mémoire minimale pour l'addition
        additionArr.length = maxLength; // On assigne la longueur du nombre
        addition(nombre1, nombre2, &additionArr, base);

        affichage(nombre1.chaine, nombre1.length, base);
        printf(" + ");
        affichage(nombre2.chaine, nombre2.length, base);
        printf(" = ");
        affichage(additionArr.resultat, additionArr.length, base);
        printf("\n");

        free(additionArr.resultat);
    }

    if( operation == '-' ) {
        resultat_t soustractionArr;
        soustractionArr.resultat = malloc(maxLength);   //Alloue la mémoire minimale pour la soustraction
        soustractionArr.length = maxLength;             // Assigne la longueur du nombre

        char plusOuMoins[5];

        /* Condition pour définir un resultat positive ou negative */
        if (!intcmp(nombre1, nombre2)) {
            soustraction(nombre2, nombre1, &soustractionArr, base);
            strcpy(plusOuMoins, " = -");
        } else {
            soustraction(nombre1, nombre2, &soustractionArr, base);
            strcpy(plusOuMoins, " = ");
        }

        affichage(nombre1.chaine, nombre1.length, base);
        printf(" - ");
        affichage(nombre2.chaine, nombre2.length, base);
        printf("%s", plusOuMoins);
        
        if (soustractionArr.length == 0) { // Si nombre1 - nombre2 == 0
            printf("0");
        } else {
            affichage(soustractionArr.resultat, soustractionArr.length, base);
        }
        printf("\n");

        free(soustractionArr.resultat);
    }

    if( operation == '*' ) {
        resultat_t multiplicationArr;
        multiplicationArr.resultat = malloc(nombre1.length + nombre2.length);
        multiplicationArr.length = (nombre1.length + nombre2.length);

        /* On initialise chaque case memoire par un 0 pour éviter des erreurs de calculs */
        for(int i = 0; i < multiplicationArr.length; i++) {
            multiplicationArr.resultat[i] = 0;
        }

        multiplication(nombre1, nombre2, &multiplicationArr, base);

        affichage(nombre1.chaine, nombre1.length, base);
        printf(" * ");
        affichage(nombre2.chaine, nombre2.length, base);
        printf(" = ");
        affichage(multiplicationArr.resultat, multiplicationArr.length, base);
        printf("\n");

        free(multiplicationArr.resultat);
    }

    if( operation == '/' || operation == 37) {
        if(!intcmp(nombre1, nombre2)) {
            affichage(nombre1.chaine, nombre1.length, base);
            printf(" / ");
            affichage(nombre2.chaine, nombre2.length, base);
            printf(" = ");
            printf(" %c \n", base.chaineBase[0]);
            printf("Reste: ");
            affichage(nombre1.chaine, nombre1.length, base);

            fclose(fa);
            return 0;
        }

        if (nombre2.length == 1 && nombre2.chaine[0] == 1) {
            printf("Diviser par 1 n'a aucun interêt, veuillez choisir un autre nombre !\n");
            fclose(fa);
            return 0;
        }

        resultat_t divisionArr;
        divisionArr.resultat = malloc(maxLength);   // Case mémoire minimale pour une division
        divisionArr.length = maxLength;             // Longueur minimale pour une division

        /* On initialise chaque case memoire par un 0 pour éviter des erreurs de calculs */
        for(int i = 0; i < divisionArr.length; i++) {
            divisionArr.resultat[i] = 0;
        }

        division(nombre1, nombre2, &divisionArr, base);
        affichage(nombre1.chaine, nombre1.length, base);
        printf(" / ");
        affichage(nombre2.chaine, nombre2.length, base);
        printf(" = ");
        affichage(divisionArr.resultat, divisionArr.length, base);
        free(divisionArr.resultat);
    }
    fclose(fa);
    return 0;
}

int verificationDuNombre(nombre_t nombre, base_t base) {
    for(int i = 0, j = 0; i < nombre.length; i++) {
        for(j = 0; j < base.base; j++) {
            if(nombre.chaine[i] == base.chaineBase[j]) {
                break;
            }
        }
        if(j == base.base) {
            return 0;
        }
    }
    return 1;
}

/* Retourne 1 si nombre1 >= nombre 2, 0 sinon */
int intcmp(nombre_t nombre1, nombre_t nombre2) {
    if(nombre1.length < nombre2.length) return 0;
    
    if (nombre1.length == nombre2.length) {
        for(int i = nombre1.length - 1; i >= 0; i--) {
            if (nombre1.chaine[i] > nombre2.chaine[i]) break;
            if (nombre1.chaine[i] < nombre2.chaine[i]) return 0;
            if (nombre1.chaine[i] == nombre2.chaine[i] && nombre1.chaine[i-1] < nombre2.chaine[i-1] && (i-1) >= 0) return 0;
        }
    }
    return 1;
}

char convertir_chiffre_a_caractere(int chiffre, base_t base) {
    int i = 0;
    for(; i < 95; i++) {
        if (chiffre == i) {
            break;
        }
    }
    return base.chaineBase[i];
}

void affichage(char * tableau, int longueur, base_t base) {
    for(int i = longueur - 1; i >= 0; i--) {
        /*On affiche le caractère relative à la base de l'utilisateur*/
        printf("%c", convertir_chiffre_a_caractere(tableau[i], base));
        if(i == 0) {
            break;
        }
        if( longueur % 3 == 0 && i % 3 == 0){
            printf(" ");
        }
        if ( longueur % 3 == 1 && i % 3 == 0) {
            printf(" ");
        }
        if( longueur % 3 == 2 && i % 3 == 0) {
            printf(" ");
        }
    }
}

void reverse(char * array, int length) {
    int temp = 0;
    for(int i = 0, n = length / 2; i < n; i++) {
        temp = array[i];
        array[i] = array[length - 1 - i];
        array[length - 1 - i] = temp;
    }
}

void returnChiffre(nombre_t * nombre, base_t base) {
    int i;
    for(i = 0; i < nombre->length; i++) {
        for(int j = 0; j < base.base; j++) {
            if(nombre->chaine[i] == base.chaineBase[j]) {
                nombre->chaine[i] = j;
            }
        }
    }

    /* On remplit les autres cases mémoires par des 0 pour éviter des erreurs de calcul */
    for(;i < 1000; i++) {
        nombre->chaine[i] = 0;
    }

    /* On retourne le nombre pour faciliter les opérations */
    reverse(nombre->chaine, nombre->length);
}

void addition(nombre_t nombre1, nombre_t nombre2, resultat_t * resultat, base_t base) {
    int retenue = 0;
    int max = nombre1.length >= nombre2.length ? nombre1.length : nombre2.length;
    int i = 0;

    /* Cette boucle effectue l'addition comme en école primaire */
    while (1) {
        if(nombre1.chaine[i] + nombre2.chaine[i] + retenue >= base.base) {
            resultat->resultat[i] = (nombre1.chaine[i] + nombre2.chaine[i] + retenue) % base.base;
        } else {
            resultat->resultat[i] = (nombre1.chaine[i] + nombre2.chaine[i] + retenue);
        }
        retenue = (nombre1.chaine[i] + nombre2.chaine[i] + retenue) / base.base;
        i++;
        if(i == max && retenue != 0) { //Le dernier indice dispose d'un reste !
            resultat->length++;
            realloc(resultat->resultat, resultat->length); //Pour acceuilir un chiffre de plus
            resultat->resultat[i] = retenue;

            break;
        } else if(i == max && retenue == 0) { // Si on est arrivé à la fin de l'opération et que le reste est nul
            break;
        }
    }
}

void soustraction(nombre_t nombre1, nombre_t nombre2, resultat_t * resultat, base_t base) {
    int i = 0;
    int max = nombre1.length >= nombre2.length ? nombre1.length : nombre2.length;

    /* Cette boucle effectue la soustraction comme en école primaire */
    while(i < max) {
        if(nombre1.chaine[i] - nombre2.chaine[i] < 0) {
            nombre1.chaine[i] += base.base;
            nombre2.chaine[i+1] += 1;
        }
        resultat->resultat[i] = nombre1.chaine[i] - nombre2.chaine[i];
        i++;
    }
    while(i >= 0 && resultat->resultat[i-1] == 0) {
        resultat->length--;
        i--;
    }
}

void multiplication(nombre_t nombre1, nombre_t nombre2, resultat_t * resultat, base_t base) {
    int reste = 0;
    int i; // Position du chiffre au nombre1
    int j; // Position du chiffre au nombre2
    nombre_t ligneResultat;
    ligneResultat.length = nombre1.length;

    /*
    Multiplication comme en primaire
    nombre1 est le premier nombre de la table de multiplication
    nombre2 est le deuxième nombre de la table de multiplication
    */

    for (j = 0; j < nombre2.length; j++) {
        ligneResultat.length += j;
        for(int k = 0; k < j; k++) {
            ligneResultat.chaine[k] = 0;
        }
        for(i = 0; i < nombre1.length; i++) {
            ligneResultat.chaine[i + j] = ((nombre2.chaine[j] * nombre1.chaine[i]) + reste) % base.base;
            reste = ((nombre2.chaine[j] * nombre1.chaine[i]) + reste) / base.base;

            if (i == nombre1.length - 1 && reste != 0) {
                ligneResultat.chaine[i + j + 1] = reste;
                ligneResultat.length++;
            }

        }

        additionnerDeuxNombres(ligneResultat, resultat, base.base);
        ligneResultat.length = nombre1.length;
        reste = 0;
    }
}

void additionnerDeuxNombres(nombre_t nombre1, resultat_t * resultat, int base) {
    int reste = 0;
    int temp = 0;
    for(int i = 0; i < nombre1.length; i++) {
        temp = resultat->resultat[i];
        if (nombre1.chaine[i] + resultat->resultat[i] + reste >= base) {
            resultat->resultat[i] = (nombre1.chaine[i] + resultat->resultat[i] + reste) % base;
        } else {
            resultat->resultat[i] = (nombre1.chaine[i] + resultat->resultat[i] + reste);
        }
        reste = (temp + nombre1.chaine[i] + reste) / base;
        if(i == nombre1.length - 1 && reste != 0) {
            while(reste != 0) {
                i++;
                temp = resultat->resultat[i];
                resultat->resultat[i] = (resultat->resultat[i] + reste) % base;
                reste = (temp + reste) / base;
            }
        }
    }
}

/* Retourne 1 si nombre >= nombre 2, 0 sinon */
int intcmpChaine(char * nombre, int length, nombre_t nombre2) {
    if(length < nombre2.length) return 0;
    
    if (length == nombre2.length) {
        for(int i = length - 1; i >= 0; i--) {
            if (nombre[i] > nombre2.chaine[i]) break;
            if (nombre[i] < nombre2.chaine[i]) return 0;
            if (nombre[i] == nombre2.chaine[i] && nombre[i-1] < nombre2.chaine[i-1] && (i-1) >= 0) return 0;
        }
    }
    return 1;
}

void division(nombre_t nombre1, nombre_t nombre2, resultat_t * resultat, base_t base) {
    int longueurReference = nombre1.length; // represente la longueur totale du nombre
    nombre1.length = 1;

    int i = 0, k = 0, nombreDeZero = 0;
    while ( k < 2 ) {
        if ( !intcmpChaine((nombre1.chaine + longueurReference - nombre1.length), nombre1.length, nombre2) ) {
            resultat->resultat[i] = 0;
            nombre1.length++;
        } else {
            nombreDeZero = nombre1.length;
            resultat->resultat[i] = multipleResultant((nombre1.chaine + longueurReference - nombre1.length), &nombre1.length, nombre2, base);
            longueurReference = longueurReference - (nombreDeZero - nombre1.length); // On réduit la longueur à partir des 0 inutiles derriere le nombre
            nombre1.length++; // On ajoute 1, cela représente l'action de faire descendre un nombre dans la file
        }
        // printf("resultat ! %i\n", resultat->resultat[i]);

        /* 
        La division se termine, lorsque la valeur de l'addresse de la chaine principale,
        est exactement celle de la chaine secondaire lors de la division.
        Il faut que cette condition soit validé deux fois.
        */
        if ( nombre1.chaine == nombre1.chaine + longueurReference - nombre1.length || nombre1.chaine - 1 == nombre1.chaine + longueurReference - nombre1.length) {
            k++;
        }

        i++;
    }

    reverse(resultat->resultat, resultat->length);
    int zero = resultat->length - 1;

    while ( resultat->resultat[zero] == 0 ) {
        resultat->length--;
        zero--;
    }

    printf("Reste: ");
    affichage(nombre1.chaine, nombre1.length, base);
    printf("\n");

}

int multipleResultant(char * nombre1, int * longueur1, nombre_t nombre2, base_t base) {
    int retenue = 0, i = 0, multiple = 0; // On déclare une retenue pour ne pas modifier la structure du nombre2
    // int temp = 0;
    while( intcmpChaine(nombre1, *longueur1, nombre2) ) {
        retenue = 0;

        for (i = 0; i < *longueur1; i++ ) {
            // temp = nombre1[i];
            if ( (nombre1[i] - (nombre2.chaine[i] + retenue) ) < 0 ) {
                nombre1[i] = nombre1[i] + base.base - (nombre2.chaine[i] + retenue);
                // printf("(%i + %i) - (%i + %i) = %i\n", temp, base.base, nombre2.chaine[i], retenue, nombre1[i]);
                retenue = 1;
            } else {
                nombre1[i] = nombre1[i] - (nombre2.chaine[i] + retenue);
                // printf("%i - (%i + %i) = %i\n", temp, nombre2.chaine[i], retenue, nombre1[i]);
                retenue = 0;
            }
        }
        while( nombre1[i - 1] == 0 && i > 0) {
            *(longueur1) = *(longueur1) - 1;
            i--;
        }
        multiple++;
    }

    return multiple;
}