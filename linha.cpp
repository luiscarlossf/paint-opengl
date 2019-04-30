//*********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica 
//  Assunto: Rasterizacao e Transformacoes Geometricas
//
//  Autor: Prof. Laurindo de Sousa Britto Neto
//*********************************************************************

// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Variaveis Globais
bool click1 = false, click2 = false;

int x_1,y_1,x_2,y_2;

//variáveis usadas para calcular primeiro octante
bool declive = false, simetrico = false;

int width = 512, height = 512; //Largura e altura da janela

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
    int x;
    int y;
    ponto * prox;
};

// Lista encadeada de pontos
// indica o primeiro elemento da lista
ponto * pontos = NULL;

// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
ponto * pushPonto(int x, int y){
	ponto * pnt;
	pnt = new ponto;
	pnt->x = x;
	pnt->y = y;
	pnt->prox = pontos;
	pontos = pnt;
	return pnt;
}

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
ponto * popPonto(){
	ponto * pnt;
	pnt = NULL;
	if(pontos != NULL){
		pnt = pontos->prox;
		delete pontos;
		pontos = pnt;
	}
	return pnt;
}

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

// Funcao que implementa o Algoritmo Imediato para rasterizacao de retas
void retaImediata(double x1,double y1,double x2,double y2);
void primeiroOctante(int * x1, int * y1, int * x2, int * y2);
void octanteInverso(int * x, int * y);
void bresenham(int x1, int y1, int x2, int y2);
// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();



// Funcao Principal do C
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Rasterizacao"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    glutMainLoop(); // executa o loop do OpenGL
    return 0; // retorna 0 para o tipo inteiro da funcao main();
}


// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

void reshape(int w, int h)
{
	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo GL_MODELVIEW (n�o pretendemos alterar a projec��o
   // quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    click1 = true; //Para redesenhar os pixels selecionados
    click2 = false;
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;
    }
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
            if(click1){
                click2 = true;
                x_2 = x;
                y_2 = height - y;
                printf("x2y2(%d,%d)\n",x_2,y_2);
                glutPostRedisplay();
            }else{
                click1 = true;
                x_1 = x;
                y_1 = height - y;
                printf("x1y1(%d,%d)\n",x_1,y_1);
            }
         }
         break;
/*
      case GLUT_MIDDLE_BUTTON:
         if (state == GLUT_DOWN) {
            glutPostRedisplay();
         }
         break;
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {
            glutPostRedisplay();
         }
         break;
*/
      default:
         break;
   }
}


// Funcao usada na funcao callback para desenhar na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o Buffer de Cores
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    

    if(click1 && click2){
        bresenham(x_1, y_1, x_2, y_2);
        drawPontos();
        click1 = false;
        click2 = false;
    }

    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

//Funcao que desenha os pontos contidos em uma lista de pontos
void drawPontos(){
    ponto * pnt;
    pnt = pontos;
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        while(pnt != NULL){
            glVertex2i(pnt->x,pnt->y);
            pnt = pnt->prox;
        }
    glEnd();  // indica o fim do desenho
}

void retaImediata(double x1,double y1,double x2,double y2){
    double m, b, yd, xd;
    double xmin, xmax,ymin,ymax;
    //Armazenando os extremos para desenho
    pontos = pushPonto((int)x1,(int)y1);
    pontos = pushPonto((int)x2,(int)y2);

    if(x2-x1 != 0){ //Evita a divis�o por zero
        m = (y2-y1)/(x2-x1);
        b = y1 - (m*x1);

        if(m>=-1 && m <= 1){ // Verifica se o declive da reta tem tg de -1 a 1, se verdadeira calcula incrementando x
            xmin = (x1 < x2)? x1 : x2;
            xmax = (x1 > x2)? x1 : x2;

            for(int x = (int)xmin+1; x < xmax; x++){
                yd = (m*x)+b;
                yd = floor(0.5+yd);
                pontos = pushPonto(x,(int)yd);
            }
        }else{ // Se menor que -1 ou maior que 1, calcula incrementado os valores de y
            ymin = (y1 < y2)? y1 : y2;
            ymax = (y1 > y2)? y1 : y2;

            for(int y = (int)ymin + 1; y < ymax; y++){
                xd = (y - b)/m;
                xd = floor(0.5+xd);
                pontos = pushPonto((int)xd,y);
            }
        }

    }else{ // se x2-x1 == 0, reta perpendicular ao eixo x
        ymin = (y1 < y2)? y1 : y2;
        ymax = (y1 > y2)? y1 : y2;
        for(int y = (int)ymin + 1; y < ymax; y++){
            pontos = pushPonto((int)x1,y);
        }
    }
}

void primeiroOctante(int * x1, int * y1, int * x2, int * y2){
    printf("Primeiro octante: (%d,  %d) - (%d, %d) -> ", (*x1), (*y1), (*x2), (*y2));
    declive = false;
    simetrico = false;
    int delta_x = (*x2) - (*x1);
    int delta_y = (*y2) - (*y1);
    //Primeiro teste - Simetria
    if ((delta_x * delta_y) < 0){
        simetrico = true;
        (*y1) = -(*y1);
        (*y2) = -(*y2);
        delta_y = -delta_y;
    }

    //Segundo teste - Declive
    if(abs(delta_x) < abs(delta_y)){
        int aux;
        declive = true;
        aux = (*x1);
        (*x1) = (*y1);
        (*y1) = aux;
        aux = (*x2);
        (*x2) = (*y2);
        (*y2) = aux;
        aux = delta_x;
        delta_x = delta_y;
        delta_y = aux;
    }
    //Terceiro teste
    if((*x1) > (*x2)){
        int aux1, aux2;
        aux1 = (*x1);
        aux2 = (*y1);
        (*x1) = (*x2);
        (*y1) = (*y2);
        (*x2) = aux1;
        (*y2) = aux2;
        delta_x = -delta_x;
        delta_y = -delta_y;
    }
    printf("> (%d,  %d) - (%d, %d)\n", (*x1), (*y1), (*x2), (*y2));
}

void octanteInverso(int *x, int *y){
    printf("Invertendo: (%d , %d) -> ", (*x),(*y));
    if(declive){
        int aux = (*x);
        (*x) = (*y);
        (*y) = aux;
    }

    if(simetrico){
        (*y) = -(*y);
    }
    printf("-> (%d , %d)\n", (*x),(*y));
}

void bresenham(int x1, int y1, int x2, int y2){
    int aux1, aux2;
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int d = (2 * delta_y) - delta_x;
    int incE = 2 * delta_y;
    int incNE = 2 * (delta_y - delta_x);
    double m = delta_y / delta_x;
    int x , y;
    bool p_octante = (m >= 0) && (m <= 1);
    
    /*
    //Verifica se está no primeiro octante
    if(!p_octante){
        primeiroOctante(&x1, &y1, &x2, &y2);
    }

    //Armazenando os extremos para desenho
    if(!p_octante)
        octanteInverso(&x1, &y1);
    */
    x = x1;
    y = y1;
    pontos = pushPonto(x, y);
    while(x < x2){
            //incE
            if(d <= 0){
                d += incE;
            }else{ //incNE
                d += incNE;
                y += 1;
            }
            x += 1;
            /*
            //Transformação para o octante original
            if(!p_octante)
                octanteInverso(&x, &y);
            */
            pontos = pushPonto(x, y);
    }
}




