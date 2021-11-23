#include <Gamebuino-Meta.h>

/*
Ce programme est un jeu pour la GameBuino META
https://gamebuino.com/fr

inspiré d'un concept de jeu de Brackeys
https://www.youtube.com/watch?v=p8MzsDBI5EI
*/

const SaveDefault defaults[] = {
    {0, SAVETYPE_INT, 0, 0},
};

//La distance est initialisée a 2 afin de laisser au joueur le temps de se préparer
double distance = 2;

//gamespeed est augmentée a chaques points gagnés jusqu' a atteindre la valeur de 3
//gameSpeed permet d'augmenter la vitesse des hexagones
double gameSpeed = 1;

//gap corespond au trou dans l'hexagone
int gap = random(1, 8);

//récuperation des informations d'affichage de l'écran
int Xsize = gb.display.width();
int Ysize = gb.display.height();

//Coordonées initiales du joueur
int x = Xsize / 2;
int y = Ysize / 2;

//initialisation du score
int score = 0;

//highscore_beaten permet de changer la couleur sur l'ecran de game over si le record a été atteint
int hisghscore_beaten = 0;
int highscore;

//state correspond a l'état du jeu 0=en jeu ; 1=game over
int state = 0;

//cette fonction permet de calculer la distance au carré entre deux points
//Merci JMP ;)
inline int getDis(int x1, int y1, int x2, int y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void setup()
{
    gb.begin();
    gb.save.config(defaults);
}

void loop()
{
    //rediriger vers la fonction game ou gameOver en fonction de l'état du jeu
    if (state == 0)
    {
        game();
    }
    else if (state == 1)
    {
        gameOver();
    }
}

void gameOver()
{
    while (!gb.update())
        ;

    gb.display.clear();
    gb.display.setColor(BEIGE);
    gb.display.setFontSize(2);

    //afficher tous les textes centrés sur l'axe X
    //le centrage se fait comme cela (Xsize/2)-((gb.display.getFontWidth())*text.length() / 2

    String text = "GAME OVER";
    gb.display.setCursor((Xsize / 2) - (gb.display.getFontWidth() * text.length()), Ysize / 2 - 20); //Note : pour ce texte la division par 2 pour calculer le centrage X n'est pas utile car la taille de la police est de 2
    gb.display.print(text);

    text = "press A to restart";
    gb.display.setFontSize(0.2);
    gb.display.setCursor((Xsize / 2) - ((gb.display.getFontWidth()) * text.length() / 2), Ysize - gb.display.getFontHeight());
    gb.display.print(text);

    gb.display.setColor(GRAY);
    text = "Score :" + String(score);
    gb.display.setFontSize(0.3);
    gb.display.setCursor((Xsize / 2) - (gb.display.getFontWidth() * text.length() / 2), Ysize / 2);
    gb.display.print(text);

    gb.display.setColor(DARKGRAY);

    //Si le meilleur score a été atteint alors la couleur du texte est verte au lieu de gris foncé
    if (hisghscore_beaten == 1)
    {
        gb.display.setColor(GREEN);
    }
    text = "High :" + String(highscore);
    gb.display.setFontSize(0.3);
    gb.display.setCursor((Xsize / 2) - (gb.display.getFontWidth() * text.length() / 2), Ysize / 2 + gb.display.getFontHeight());
    gb.display.print(text);

    //Verifier pour redémarrer le jeu
    if (gb.buttons.pressed(BUTTON_A))
    {
        score = 0;
        //passer l'état de jeu a 0 (jeu en cours)
        state = 0;
        //La distance du premier hexagone repasse a 2 afin de laisser un leger temps au joueur avant de démarrer la partie
        distance = 2;
        //highscore_beaten repasse a 0 car le score a été battu
        hisghscore_beaten = 0;
        //Les coordonées du joueur sont aussi réinitialisées
        int x = Xsize / 2;
        int y = Ysize / 2;
        gameSpeed=1;
        gb.sound.playOK();
    }
}

void game()
{
    while (!gb.update())
        ;
    gb.display.clear();

    //déplacement de l'hexagone en fonction de la vitesse de jeu
    distance = distance - 0.01 * gameSpeed;

    //mouvements
    if (gb.buttons.timeHeld(BUTTON_UP) > 1 && y > 0)
    {
        y = y - 2;
    }
    if (gb.buttons.timeHeld(BUTTON_RIGHT) > 1 && x < Xsize)
    {
        x = x + 2;
    }
    if (gb.buttons.timeHeld(BUTTON_DOWN) > 1 && y < Ysize)
    {
        y = y + 2;
    }
    if (gb.buttons.timeHeld(BUTTON_LEFT) > 1 && x > 0)
    {
        x = x - 2;
    }

    //dessiner le joueur
    gb.display.setColor(BLUE);
    gb.display.fillCircle(x, y, 2.5);

    //dessiner les côtés de l'hexagone
    gb.display.setColor(RED);
    for (int i = 0; i <= 8; i++)
    {
        //cette condition permet de laisser un trou dans l'hexagone afin de laisser le passage au joueur
        if (gap != i)
        {
            double angle = 2 * PI / 8;
            gb.display.drawLine(Xsize / 2 + sin(angle * i) * Xsize * distance, Ysize / 2 + cos(angle * i) * Xsize * distance, Xsize / 2 + sin(angle * (i + 1)) * Xsize * distance, Ysize / 2 + cos(angle * (i + 1)) * Xsize * distance);
        }
    }

    //verification de la collision
    //on parcours un carré autour du joueur
    for (int _x = -3; _x <= 3; _x++)
    {
        for (int _y = -3; _y <= 3; _y++)
        {
            //on verifie si le pixel du carré parcouru appartient au joueur
            if (getDis(x + _x, y + _y, x, y) <= 6)
            {
                //si tel est le cas on vérifie qu'une ligne rouge n'y soit pas présent
                if (gb.display.getPixelColor(x + _x, y + _y) == RED)
                {
                    //Si tel est le cas procédure de mort
                    //récuperation du meilleur score
                    highscore = gb.save.get(0);

                    //si le meilleur score est battu
                    if (score > highscore)
                    {
                        //sauvegarder le meilleur score
                        gb.save.set(0, score);
                        //informer la fonction de l'ecran de game over que le meilleur score a été battu
                        hisghscore_beaten = 1;

                        //afficher le popup maintenant permet d'éviter qu'il soit bloqué dans la boucle gameOver ce qui empêcherais son affichage
                        gb.gui.popup("New High Score", 50);
                    }
                    gb.sound.playCancel();
                    //passer l'état de jeu a  0 (gameOver)
                    state = 1;
                }
            }
        }
    }

    //verifier que le distance de l'hexagone ne soit pas a 0 (arrivé au centre)
    if (distance <= 0)
    {
        //si tel est le cas réinitialiser la distance pour créer un nouvel hexagone
        //NOTE : la dsitance est initialisée a 0.8 et pas a 1 car le rendu est meilleur
        distance = 0.8;

        //augmentation de la vitesse de jeu si celle ci est inferieur ou égal a 3
        //NOTE : cela permet que le jeu soit toujours jouable en évitant des vitesses de jeu imbattables
        if (gameSpeed <= 3)
        {
            gameSpeed = gameSpeed + 0.05;
        }
        //réinitialiser l'ouverture de l'hexagone
        gap = random(1, 8);
        //incrémenter le score
        score++;
        gb.sound.playTick();
    }
    //afficher le score actuel
    gb.display.setColor(GRAY);
    gb.display.print(score);
}