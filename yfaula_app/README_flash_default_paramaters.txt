*******************************************************************************
*                       INFORMATIONS GENERALES                                *
*******************************************************************************

Contact
-------
mail : yannick.faula at insa-lyon.fr
tel : 06 46 24 90 11

Description générale du Logiciel
--------------------------------
App permettant la détection de lignes sur des images.

*******************************************************************************
*                       AIDE SUR LES PARAMETRES                               *
*******************************************************************************

ANGLE_SEARCH            : Angle de recherche des points détectés autour d'un point considéré. 
                          Plus l'angle est petit, plus la ligne détectée sera droite.
                          L'angle est en dégré

BETA                    : Pas nécessaire pour les cartes historiques

BLACK_ON_WHITE?         : Booléen pour déterminer si le traitement se fait normalement (valeur 1) pour
                         une détection d'une ligne noir sur fond blanc,
                         ou en vidéo inverse (valeur <=0 ) pour une détection d'une ligne blanche sur fond noir

DIFFERENCE_LEVEL_SIDES  : Pas nécessaire pour les cartes historiques

MIN_SIZE_GRAPH          : Taille minimale APPROXIMATIVE d'un bout d'une ligne (un graphe) à afficher.
                         La taille est en nombre de pixels détectés dans un graphe.
                         Les graphes inférieures ne seront pas pris en compte dans la fusion.
                         Si on cherche des lignes très fine et difficillement détectable,
                         il vaut mieux garder une petite taille entre 5-20 pixels.
                         
NB_FRIENDS_MAX          : Nombre d'amis pris en compte dans la liaison entre pixels. 
                         Plus de points entrainent une meilleure détection mais ralentit un peu la détection.
                         Il ne s'agit pas d'un parametre primordiale pour les cartes historiques.
                         Entre 5 et 12 amis max est accepatble.

THRESHOLD_LINE_CRITER   : Un paramètre de seuil pour éliminer des fausses détections. Si on veut pas la prendre en compte,
                         on donnera une valeur très élevé (au moins 100).
                         Pas trop y touché.
                         Sinon on testera des valeurs autour de 10, 20 ou 30 selon le type de murs notamment.

THRESHOLD_REL_EIGEN     : Meme fonction que le paramètre précédent. Ce paramètre prend des flottants entre 0 et 1.
                         Il détermine la dispersion d'un graphe représentant une ligne. Si les points sont très dispersés,
                         ce graphe aura une valeur entre 0.5 et 0. Si un graphe est une ligne droite, on sera très proche de 1.
                         Ainsi, on donne un seuil pour éliminer certains graphes qui peuvent être de fausses détections.

X_FACTOR_X2ORIGIN       : Valeur flottante entre 0.1 et 1. Utilisé avec le paramètre FACTOR_INTERVAL.
                          Facteur de résolution de départ pour la détection. Si ce paramètre est à 0.75,
                          le traitement sera à 75 pourcent de la résolution d'origine 
                          puis un traitement tout les FACTOR_INTERVAL jusqu à la résolution d'origine.
                          REM : Pas plus de 3 niveaux de résolution pour le traitement des cartes historiques.

FACTOR_INTERVAL         : flottant indiquant l'intervalle du facteur de résolution à prendre en compte avec X_FACTOR_X2ORIGIN.

ID_PALIER               : 0, 1, 2 ou 3. Quatre niveaux représentant le périmètre et le nombre minimum de recherche 
                          des pixels détectés autour d'un point considéré.
                          Pas très utile pour les cartes.



Pour la détection de lignes sur les cartes historiques, on peut partir du paramétrage suivant:

ANGLE_SEARCH = 22.5
BETA = 8
BLACK_ON_WHITE? = 0
FACTOR_INTERVAL = 0.25
ID_PALIER = 0
MIN_SIZE_GRAPH = 30
NB_FRIENDS_MAX = 10
THRESHOLD_LINE_CRITER = 100
THRESHOLD_REL_EIGEN = 0.6
X_FACTOR_X2ORIGIN = 0.75