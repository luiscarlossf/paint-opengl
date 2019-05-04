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

int x_0, y_0, x_1,y_1,x_2,y_2;

int shape = 4;

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

// Lista encadeada de pontos clicados
// indica o primeiro elemento da lista
ponto * cliques = NULL;

//Conta a quantidade de elementos da lista
int cliques_cont = 0;

// Funcao para armazenar um ponto de clique na lista
// Armazena como uma Pilha (empilha)
ponto * pushClique(int x, int y){
	ponto * pnt;
	pnt = new ponto;
	pnt->x = x;
	pnt->y = y;
	pnt->prox = cliques;
	cliques = pnt;
    printf("Inserindo pontoClique: (%d, %d)\n", cliques->x, cliques->y);
	return pnt;
}

// Funcao para desarmazenar um ponto de clique na lista
// Desarmazena como uma Pilha (desempilha)

ponto popClique(){
    ponto * pnt;
    ponto saida;
	pnt = NULL;
	if(cliques != NULL){
        saida.x = cliques->x;
        saida.y = cliques->y;
		pnt = cliques->prox;
		delete cliques;
		cliques = pnt;
        printf("PontoClique: (%d, %d)\n", saida.x, saida.y);
	}
	return saida;
}

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

//Funcao desenha reta
void bresenham(int x1, int y1, int x2, int y2);

//Funcao desenha quadrilátero
void quadrilatero(int x1, int y1, int x2, int y2);

//Funcao desenha triangulo
void triangulo(int x1, int y1, int x2, int y2, int x3, int y3);

//Funcao desenha circunferencia
void circunferencia( int cx, int cy, int raio);

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
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case 27: // codigo ASCII da tecla ESC
            while(popPonto() != NULL);
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
            cliques = pushClique(x, height - y);
            cliques_cont++;
            switch (shape)
            {
            //Reta
            case 0:
                if(cliques_cont == 2){
                    glutPostRedisplay();
                }
                break;
            //Quadrilátero
            case 1:
                if(cliques_cont == 2){
                    glutPostRedisplay();
                }
                break;
            //Triangulo
            case 2:
                if(cliques_cont == 3){
                    glutPostRedisplay();
                }
                break;
            //Poligono
            case 3:
                if(cliques_cont == 1){
                    x_1 = x;
                    y_1 = height - y;
                    x_2 = x;
                    y_2 = height- y;
                    printf("x2y2(%d,%d)\n",x_2,y_2);
                    //glutPostRedisplay();
                }else if (cliques_cont > 1){
                    x_1 = x_2;
                    y_1 = y_2;
                    x_2 = x;
                    y_2 = height - y;
                    printf("x1y1(%d,%d)\n",x_1,y_1);
                }

                glutPostRedisplay();
                break;
            //Circunferência
            case 4:
                if(cliques_cont == 1){
                    x_1 = x;
                    y_1 = height - y;
                }else if(cliques_cont == 2){
                    x_2 = x;
                    y_2 = height - y;
                    glutPostRedisplay();
                }
                break;
            
            default:
                break;
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

void mouse_m(int x, int y){
}

// Funcao usada na funcao callback para desenhar na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o Buffer de Cores
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    int i;
    ponto aux;
    switch (shape)
            {
            //Reta
            case 0:
                ponto cr[2]; //os três cliques
                if(cliques_cont == 2){
                    for(i = 0; i < 2; i++){
                        cr[i] = popClique();
                        cliques_cont--;
                    }
                    bresenham(cr[0].x, cr[0].y, cr[1].x, cr[1].y);
                    drawPontos();
                }
                break;
            //Quadrilátero
            case 1:
                ponto cq[2]; //os três cliques
                if(cliques_cont == 2){
                    for(i = 0; i < 2; i++){
                        cq[i] = popClique();
                        cliques_cont--;
                    }
                    quadrilatero(cq[0].x, cq[0].y, cq[1].x, cq[1].y);
                    drawPontos();
                }
                break;
            //Triangulo
            case 2:
                ponto ct[3]; //os três cliques
                if(cliques_cont == 3){
                    for(i = 0; i < 3; i++){
                        ct[i] = popClique();
                        cliques_cont--;
                    }
                    triangulo(ct[0].x, ct[0].y, ct[1].x, ct[1].y, ct[2].x, ct[2].y);
                    drawPontos();
                }
                break;
            //Poligono
            case 3:
                if(cliques_cont == 1){
                    x_0 = x_1;
                    y_0 = y_1;
                }
                if(cliques_cont > 1){
                    if(((x_2 >= x_0 - 4) && (y_2 <= y_0 + 4)) && ((x_2 <= x_0 + 4) && (y_2 >= y_0 - 4))){
                        x_2 = x_0;
                        y_2 = y_0;
                    } 
                    bresenham(x_1, y_1, x_2, y_2);
                    drawPontos();
                    if(x_0 == x_2 && y_0 == y_2){
                        while(cliques_cont != 0){
                            aux = popClique();
                            cliques_cont--;
                        }
                        cliques_cont = 0;
                    }
                }
                break;
            //Circunferência
            case 4:
                int raio;
                if(cliques_cont == 2){
                    printf("X1(%d,%d) X2(%d, %d)\n", x_1, y_1, x_2, y_2);
                    raio = sqrt(pow((x_2 - x_1), 2) + pow((y_2 - y_1), 2));
                    circunferencia(x_1, y_1, raio);
                    drawPontos();
                    while(cliques_cont != 0){
                            aux = popClique();
                            cliques_cont--;
                    }
                }
                break;
            
            default:
                break;
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

void bresenham(int x1, int y1, int x2, int y2){
    printf("Desenhando resta. Extremos: (%d, %d) e (%d, %d)\n", x1, y1, x2, y2);
    bool declive, simetrico;
    int aux;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dx_ = dx, dy_ = dy, x1_ = x1, x2_ = x2;
    int x , y;
    
    //Redução de octante
    declive = false;
    simetrico = false;
    if ((dx * dy) < 0){
        y1 = (-1) * y1;
        y2 = (-1) * y2;
        dy = (-1) * dy;
        simetrico = true;
    }
    if (abs(dx) < abs(dy)){
        aux = x1;
        x1 = y1;
        y1 = aux;

        aux = x2;
        x2 = y2;
        y2 = aux;

        aux = dx;
        dx = dy;
        dy = aux;

        declive = true;
    }

    if(x1 > x2){
        aux = x1;
        x1 = x2;
        x2 = aux;

        aux = y1;
        y1 = y2; 
        y2 = aux;

        dx = (-1) * dx;
        dy = (-1) * dy;
    }
    
    x = x1;
    y = y1;
    int x_;
    int y_;

    //Algoritmo de Bresenham
    int d = (2 * dy) - dx;
    int incE = 2 * dy;
    int incNE = 2 * (dy - dx);
    
    while(x < x2){
        //incE
        if(d <= 0){
            d += incE;
        }else{ //incNE
            d += incNE;
            y += 1;
        }
        x += 1;
        
        //Transformação para o octante original
        x_ = x;
        y_ = y;
        if(declive == true){
            x_ = y;
            y_ = x;
        }

        if(simetrico == true){
            y_ = (-1) * y_;
        }
        pontos = pushPonto(x_, y_);
    }
}

void quadrilatero(int x1, int y1, int x2, int y2){
    printf("Desenhando quadrilátero. Topo esquerdo: (%d, %d) Base Direita: (%d, %d)\n", x1, y1, x2, y2);
    bresenham(x1, y1, x2, y1);
    bresenham(x2, y1, x2, y2);
    bresenham(x2, y2, x1, y2);
    bresenham(x1, y2, x1, y1);
    printf("\n\n");
}

void triangulo(int x1, int y1, int x2, int y2, int x3, int y3){
    printf("Desenhando quadrilátero. Extremos: (%d, %d), (%d, %d) e (%d, %d)\n", x1, y1, x2, y2, x3, y3);
    bresenham(x1, y1, x2, y2);
    bresenham(x2, y2, x3, y3);
    bresenham(x3, y3, x1, y1);
}

void circunferencia(int cx, int cy, int raio){
    printf("Desenhando Circunferência. Raio: %d, Centroide: (%d, %d)\n", raio, cx, cy);
    int d = 1 - raio;
    int de = 3;
    int dse = (-2 * raio) + 5;
    int x = 0, y = raio;
    pontos = pushPonto(0 + cx, raio + cy);
    pontos = pushPonto(0 + cx, -raio  + cy);
    pontos = pushPonto(raio + cx, 0  + cy);
    pontos = pushPonto(-raio + cx, 0 + cy);
    while(y > x){
        if( d < 0){
            d += de;
            de += 2;
            dse += 2;
        }else{
            d += dse;
            de += 2;
            dse += 4;
            y--;
        }
        x++;
        pontos = pushPonto(x + cx, y + cy);
        pontos = pushPonto(-x + cx, y + cy);
        pontos = pushPonto(x + cx, -y + cy);
        pontos = pushPonto(-x + cx, -y + cy);
        pontos = pushPonto(y + cx, x + cy);
        pontos = pushPonto(-y + cx, x + cy);
        pontos = pushPonto(y + cx, -x + cy);
        pontos = pushPonto(-y + cx, -x + cy);
    }

}



