#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <chrono>

class Player{
private:
    float posx = 0;
    float posy = 0;
    sf::RenderWindow* tela;

protected:
    sf::RectangleShape corpo{ sf::Vector2f(20.f, 30.f) };
    sf::RectangleShape cabeca{ sf::Vector2f(10.f, 10.f) };

public:
    Player(sf::RenderWindow& window, float posX = 0.f, float posY = 0.f,sf::Color cor = sf::Color::White) : tela(&window), posx(posX), posy(posY) {
        cabeca.setPosition({ posX + (corpo.getSize().x / 4.f),posY});
        corpo.setPosition({ posX,posY + cabeca.getSize().y });
		FillColor(cor);
    }
    void FillColor(sf::Color cor) {
        corpo.setFillColor(cor);
        cabeca.setFillColor(cor);
    }
    void moverPlayer(float posX = 0.f, float posY = 0.f,float velocidade = 1.f) {
        corpo.setPosition(sf::Vector2f(corpo.getPosition().x + posX * velocidade, cabeca.getPosition().y + posY * velocidade + 10.f));
        cabeca.setPosition(sf::Vector2f(cabeca.getPosition().x + posX * velocidade, cabeca.getPosition().y + posY * velocidade));
    };
    sf::Vector2f Posicao() {
        return {corpo.getPosition().x,cabeca.getPosition().y};
    }
    sf::Vector2f Tamanho() {
        return { corpo.getSize().x,(cabeca.getSize().y + corpo.getSize().y) };
    }
    void Desenhar() {
        tela->draw(corpo);
        tela->draw(cabeca);
    }
};

class Projetil {
private:
    float posX;
	float posY;
    sf::RectangleShape bala;
    sf::Vector2f direcao;
	float velocidade;
public:
    Projetil(Player& jogador, sf::Vector2f direcao = { 0.f,1.f }, float velocidade = 4.5) : direcao(direcao), velocidade(velocidade) {
        posX = jogador.Posicao().x;
        posY = jogador.Posicao().y;
        if (direcao.y != 0) {
            bala = sf::RectangleShape{ sf::Vector2f(5.f, 10.f) };
            bala.setPosition({ posX + jogador.Tamanho().x/4 + direcao.x,posY + direcao.y});
        }
        else {
            bala =  sf::RectangleShape{ sf::Vector2f(10.f, 5.f) };
            bala.setPosition({ posX + jogador.Tamanho().x / 4 + direcao.x,posY + direcao.y });
        }
		
    }
    void Desenhar(sf::RenderWindow& tela) {
		tela.draw(bala);
        bala.move({ direcao.x * velocidade,direcao.y * velocidade });
    }
    bool NaTela(sf::RenderWindow& tela) {
        if ((bala.getPosition().x <= tela.getSize().x) && (direcao.x == 1.f)) {
			return true;
        }
        else if ((bala.getPosition().x >= 0) && (direcao.x == -1.f)) {
            return true;
        }else if ((bala.getPosition().y >= 0) && (direcao.y == -1.f)) {
            return true;
        }else if ((bala.getPosition().y <= tela.getSize().y) && (direcao.y == 1.f)) {
            return true;
        }
        else {
            return false;
        }
    }
    sf::Vector2f Posicao() {
        return bala.getPosition();
    }
    sf::Vector2f Tamanho() {
        return bala.getSize();
    }

};

class Inimigo : public Player {
protected:
    float velocidade;
	int vida;
public:
    Inimigo(sf::RenderWindow& tela, float posX = 0.f, float posY = 0,float velocidade = 1.f,int vida=1, sf::Color cor = sf::Color::White) : Player(tela,posX,posY,cor), vida(vida), velocidade(velocidade) {
        
    }
    void MoverParaPlayer(Player& jogador) {
        if (jogador.Posicao().y < Posicao().y) {
			moverPlayer(0.f, -1.f,velocidade);
        }
        else if (jogador.Posicao().y > Posicao().y) {
            moverPlayer(0.f, 1.f, velocidade);
        }
        if (jogador.Posicao().x < Posicao().x) {
            moverPlayer(-1.f, 0.f, velocidade);
        }
        else if (jogador.Posicao().x > Posicao().x) {
            moverPlayer(1.f, 0.f, velocidade);
        }
    }
    void TomarDano(int dano=1) {
        vida -= dano;
	}
    int mostrarVida() {
        return vida;
    }
};
bool Colidiu(sf::Vector2f PosObj1, sf::Vector2f TamanhoObj1, sf::Vector2f PosObj2, sf::Vector2f TamanhoObj2) {
    if (
        (PosObj1.y + TamanhoObj1.y > PosObj2.y) &&
		(PosObj1.y < PosObj2.y + TamanhoObj2.y) &&
        (PosObj1.x + TamanhoObj1.x > PosObj2.x) && 
        (PosObj1.x < PosObj2.x + TamanhoObj2.x)) {
		return true;
    }
    else {
		return false;
    }
};

int main()
{

    int pontuacao = 0;
    int cooldown_inimigos;
    int tempo_max;
    std::vector<std::string> dificuldades{"Facil","Medio","Dificil"};
    int dificuldade_atual;
    do {
        std::cout << "Digite A Dificuldade que voce quer\nFacil[1]\nMedio[2]\nDificil[3]\n";
		std::cin >> dificuldade_atual;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Digite um numero valido!\n\n";
            continue;
        }
        if ((dificuldade_atual < 1) || (dificuldade_atual > 3)) {
            std::cout << "Digite um numero valido\n";
        }
        else {
            dificuldade_atual--;
			break;
        }
    } while (true);
    
    switch (dificuldade_atual) {
        case 0:
            cooldown_inimigos = 5;
            tempo_max = 30;
            break;
        case 1:
            cooldown_inimigos = 3;
            tempo_max = 60;
            break;
        case 2:
            cooldown_inimigos = 2;
            tempo_max = 120;
            break;
    }
    sf::Vector2u tela{ 600, 600 };
    sf::RenderWindow window(sf::VideoMode(tela), "Carinha que anda");

    Player jogador{window,tela.x/2.f,tela.y/2.f};
    auto* pointerJ = &jogador;
    float velocidade = 1.f;

    std::cout << tempo_max << " " <<cooldown_inimigos;
    
    std::vector<Inimigo> Inimigos;
    //Inimigo inimigo{ window, 300.f,300.f,2.f, sf::Color::Green };
    //Inimigos.push_back(inimigo);

    sf::Clock relogio;
    sf::Clock relogio_dt;

    std::vector<Projetil> lista_balas;
    int limite_balas = 5;

    float pos1; // usado depois to com preguica de nomear
    float pos2;

	window.setFramerateLimit(60);
    while (window.isOpen())
    {
        float dt = relogio_dt.restart().asSeconds();
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            
            if (relogio.getElapsedTime().asSeconds()>=cooldown_inimigos) {
                do {
                    pos1 = static_cast<float>(((std::rand() % 700) - 100));
                    pos2 = static_cast<float>(((std::rand() % 600) - 200));
                    if (
                        ((pos1 < (jogador.Posicao().x - 100)) || (pos1 > (jogador.Posicao().x + jogador.Tamanho().x + 100))) &&
						((pos2 < (jogador.Posicao().y - 100)) || (pos2 > (jogador.Posicao().y + jogador.Tamanho().y + 100)))
                        ) {
                        break;
                    }
                } while (true);
				int velocidade_inimigo = (std::rand() % 3) + 1;
                Inimigo inimigo{ window, pos1,pos2,static_cast<float>(velocidade_inimigo),(4-velocidade_inimigo), sf::Color::Green };
                Inimigos.push_back(inimigo);
                relogio.restart();
            }
            
            if (auto* tecla = event->getIf<sf::Event::KeyPressed>())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                    velocidade = 40.f;
                }
                else {
					velocidade = 20.f;
                }
                if ((jogador.Posicao().x < (tela.x - jogador.Tamanho().x - 2)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))) {
                    jogador.moverPlayer(10.f * dt,0.f,velocidade);
                }else if ((jogador.Posicao().x > 2) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))) {
                    jogador.moverPlayer(-10.f * dt,0.f, velocidade);
                }
                if ((jogador.Posicao().y < (tela.y - jogador.Tamanho().y - 2)) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))) {
                    jogador.moverPlayer(0.f,10.f * dt, velocidade);
                }
                else if ((jogador.Posicao().y > 2) && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))) {
                    jogador.moverPlayer(0.f,-10.f * dt, velocidade);
                }

                if ((lista_balas.size() < limite_balas)) {
                    if (tecla->code == sf::Keyboard::Key::I){
					    lista_balas.push_back(Projetil(jogador, { 0.f,-1.f }));
                    }else if (tecla->code == sf::Keyboard::Key::K) {
                        lista_balas.push_back(Projetil(jogador, { 0.f,1.f }));
                    }else if (tecla->code == sf::Keyboard::Key::L) {
                        lista_balas.push_back(Projetil(jogador, { 1.f,0.f }));
                    }else if (tecla->code == sf::Keyboard::Key::J) {
                        lista_balas.push_back(Projetil(jogador, { -1.f,0.f }));
                    }
                }
            }
            
        }
        window.clear();
        for (unsigned int i = 0; i < lista_balas.size();) {
            bool bala_destruida = false;
            if (!lista_balas[i].NaTela(window)) {
                lista_balas.erase(lista_balas.begin() + i);
                continue;
            }
            lista_balas[i].Desenhar(window);
            for (unsigned int j=0; j<Inimigos.size() ;) {
                if (Colidiu(lista_balas[i].Posicao(),lista_balas[i].Tamanho(),Inimigos[j].Posicao(),Inimigos[j].Tamanho())) {
					Inimigos[j].TomarDano();
                        
                    bala_destruida = true;
                    if (Inimigos[j].mostrarVida() <= 0) {
                        Inimigos.erase(Inimigos.begin() + j);
                        pontuacao++;
                    }
                    else {
                        j++;
                    }
                    lista_balas.erase(lista_balas.begin() + i);
                    bala_destruida = true;
                    break;
                }
                else { j++; }
            }
            if (!bala_destruida) {
                i++;
            }// codigo feio demaisssss mas n to com vontade de refatorar 
        }
        for (unsigned int j = 0; j < Inimigos.size();) {
			Inimigos[j].MoverParaPlayer(jogador);
            Inimigos[j].Desenhar();
            if (Colidiu(jogador.Posicao(), jogador.Tamanho(), Inimigos[j].Posicao(), Inimigos[j].Tamanho())) {
                window.close();
                break;
            }
            else { j++; }  
        }
        jogador.Desenhar();
        window.display();
        if (relogio.getElapsedTime().asSeconds() > tempo_max) {
            window.close();
        }
    }
    std::cout << "\033[2J\033[H";
    std::cout << "Game Over\n" << "Voce matou: " << pontuacao << " Inimigos\n" << "Sua Pontuacao foi: " << pontuacao * 1500 << " pontos" << std::endl;
    return 0;
}