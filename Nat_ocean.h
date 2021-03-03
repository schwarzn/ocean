#include <TXLib.h>
#include <memory>
#include <vector>
using namespace std;

enum objectType {STONE, GRASS, FISH, HUNTER};  //типы объектов

const int SCREEN_WIDTH  = 800,                 //ширина и высота окна
          SCREEN_HEIGHT = 600;

int randInt(int a, int b);                     //случайное значение от a до b

class COceanObject                             //виртуальный класс
{
  protected:
    int x, y, r;
    COceanObject(int x0, int y0, int r0) : x(x0), y(y0), r(r0) {}

  public:
    virtual ~COceanObject() = default;
    virtual void draw() const = 0;
    virtual void change() = 0;
    void update()                 //расчет состояния + рисование
    {
        change();
        draw();
    }
    virtual bool isDead()
    {
        return(r == 0);
    }
    bool hasCollisionWith(shared_ptr<COceanObject> pOther) const;
    virtual void collideWith(shared_ptr<COceanObject> pOther) {}
};

class CStone : public COceanObject             //камень
{
  public:
    CStone(int x0, int y0, int r0) : COceanObject(x0, y0, r0) {}

    virtual void draw() const override;        //рисование
    virtual void change() override {}
};

class CGrass : public COceanObject             //трава
{
  public:
    CGrass(int x0, int y0, int r0) : COceanObject(x0, y0, r0) {}

    virtual void draw() const override;        //рисование
    virtual void change() override             //рост
    {
        if(r < 270)
            r += 4;
    }
    virtual void collideWith(shared_ptr<COceanObject> pOther) override;
};

class CMovingObject : public COceanObject      //движущиеся объекты
{
  protected:
    double v, course;
    CMovingObject(int x0, int y0, int r0, double v0, double course0 = 0) :
    COceanObject(x0, y0, r0), v(v0), course(course0) {}

    virtual void move()                        //движение
    {
        double courseRadians = course*M_PI/180;
        x += int(round(v*cos(courseRadians)));
        y -= int(round(v*sin(courseRadians)));
        if(x - r < 0)
            x += SCREEN_WIDTH;
        if(x - r > SCREEN_WIDTH)
            x -= SCREEN_WIDTH;
        if(y - r < 0)
            y += SCREEN_HEIGHT;
        if(y - r > SCREEN_HEIGHT)
            y -= SCREEN_HEIGHT;

    }
};

class CFish : public CMovingObject             //рыба
{
  public:
    CFish(int x0, int y0, int r0, double v0, double course0) :
    CMovingObject(x0, y0, r0, v0, course0) {};

    virtual void draw() const override;        //рисование
    virtual void change() override             //движение
    {
        move();
    }
    virtual void collideWith(shared_ptr<COceanObject> pOther) override;
};

class CHunter : public CMovingObject           //хищник
{
  public:
    CHunter(int x0, int y0, int r0, int v0) :
    CMovingObject(x0, y0, r0, v0) {};

    virtual void draw() const override;       //рисование
    virtual void change() override;           //движение
    virtual void collideWith(shared_ptr<COceanObject> pOther) override;
};

class COcean                                   //океан
{
  private:
    std::vector<shared_ptr<COceanObject>> pObjects;

  public:
    void checkCollisions() const;
    void removeDead();
    void addObject(objectType type, int x0, int y0, int r0 = 0, int v0 = 0, int course0 = 0);                //добавление объекта
    void update();                                                 //обновление всех объектов
};

int randInt(int a, int b)                      //случайное значение от a до b
{
    return (a + rand()%(b - a + 1));
}

//-----------functions-----------//

bool COceanObject :: hasCollisionWith(shared_ptr<COceanObject> pOther) const               //проверка столкновения с другим объектом
{
    double distance = hypot(x - pOther->x, y - pOther->y);
    return(distance < r + pOther->r);
}

void COcean :: addObject(objectType type, int x0, int y0, int r0, int v0, int course0)     //добавление объекта
{
    shared_ptr<COceanObject> pNewObj = nullptr;
    if(type == STONE)
        pNewObj = make_shared <CStone>(x0, y0, r0);
    else if(type == GRASS)
        pNewObj = make_shared <CGrass>(x0, y0, r0);
    else if(type == FISH)
        pNewObj = make_shared <CFish>(x0, y0, r0, v0, course0);
    else
        pNewObj = make_shared <CHunter>(x0, y0, r0, v0);
    if(pNewObj)
        pObjects.push_back(pNewObj);
}

void COcean :: checkCollisions() const
{
    for(unsigned int i = 0; i < pObjects.size() - 1; i++)
    {
        for(unsigned int j = 1; j < pObjects.size(); j++)
        {
            if(pObjects[i] -> hasCollisionWith(pObjects[j]))
            {
                pObjects[i] -> collideWith(pObjects[j]);
                pObjects[j] -> collideWith(pObjects[i]);
            }
        }
    }
}

void COcean :: removeDead()                                                                //удаление объекта
{
    for(int i = pObjects.size() - 1; i >= 0; i--)
    {
        if(pObjects[i] -> isDead())
        {
            pObjects.erase(pObjects.begin() + i);
        }
    }
}

void COcean::update()                                                                      //обновление всех объектов + рисование
{
    for(auto pObj : pObjects)
        pObj -> update();
    checkCollisions();
    removeDead();
}

void CHunter :: collideWith(shared_ptr<COceanObject> pOther)                               //столкновение хищника
{
    auto pFish = dynamic_pointer_cast <CFish> (pOther);
    if(pFish)
        r += 3;
}

void CHunter :: draw() const                                                               //рисование хищника
{
    txSetColor(TX_BLACK);
    txSetFillColor(RGB(166, 19, 17));
    POINT body[9] = {{int(x - 1.5*r/4), int(y - r/8)}, {int(x - r/2  ), int(y - r/2)}, {int(x - r/4    ), int(y - 2.25*r/4)},              //тело
                     {int(x + r/4    ), int(y - r/2)}, {int(x + 3*r/4), int(y - r/4)}, {int(x + 3*r/4  ), int(y           )},
                     {int(x + r/4    ), int(y + r/2)}, {int(x - r/2  ), int(y + r/2)}, {int(x - 1.5*r/4), int(y + r/8     )}};
    txPolygon(body, 9);
    txSetFillColor(TX_RED);
    POINT pl1[6]  = {{int(x - r/2)    , int(y - r/2         )}, {int(x - 1.3*r/4), int(y - 0.75*r)}, {int(x + 1.5*r/4), int(y - 0.75*r  )},    //верхний плавник
                     {int(x + 1.3*r/4), int(y - 1.75*r/4    )}, {int(x + r/4    ), int(y - r/2   )}, {int(x - r/4    ), int(y - 2.25*r/4)}};
    txPolygon(pl1, 6);
    POINT pl2[3]  = {{int(x - r/2  ), int(y + r/2)}, {int(x - r/4), int(y + r/2)}, {int(x - r/4), int(y + 3*r/4)}};                        //нижний передний плавник
    txPolygon(pl2, 3);
    POINT pl3[3]  = {{int(x        ), int(y + r/2)}, {int(x + r/4), int(y + r/2)}, {int(x + r/4), int(y + 3*r/4)}};                        //нижний задний плавник
    txPolygon(pl3, 3);
    POINT tail[5] = {{int(x + 3*r/4), int(y - r/4)}, {int(x + r  )  , int(y - r/2)}, {int(x + 3.5*r/4), int(y - r/8)},                       //хвост
                     {int(x + r  ) , int(y + r/4 )}, {int(x + 3*r/4), int(y      )}};
    txPolygon(tail, 5);
    txSetFillColor(RGB(217, 136, 82));
    POINT head[7] = {{int(x - 1.5*r/4), int(y - r/8)}, {int(x - r/2  ), int(y - r/2    )}, {int(x - 0.8*r), int(y - 0.3*r)},               //голова
                     {int(x - r      ), int(y + r/8)}, {int(x - 0.8*r), int(y + 1.3*r/4)}, {int(x - r/2  ), int(y + r/2  )},
                     {int(x - 1.5*r/4), int(y + r/8)}};
    txPolygon(head, 7);
    txSetFillColor(TX_BLACK);                              //глаз
    txCircle(int(x - 2.5*r/4), int(y - r/8), 2);
}

void CHunter :: change()                                                                   //расчет движения хищника
{
    int xMouse = txMouseX(),
        yMouse = txMouseY();
    double dist = hypot(x - xMouse, y - yMouse);
    if(dist < 1)
        return;
    double part = v/dist;
    if(part > 1)
        part = 1;
    x += int(round(part*(xMouse - x)));
    y += int(round(part*(yMouse - y)));
}

void CFish :: collideWith(shared_ptr<COceanObject> pOther)                                 //столкновение рыбы
{
    auto pHunter = dynamic_pointer_cast <CHunter> (pOther);
    auto pGrass  = dynamic_pointer_cast <CGrass> (pOther);
    if(pHunter)
        r = 0;
    else if(pGrass)
    {
        if(r < 48)
            r += 5;
    }
}

void CFish :: draw() const                                                                 //рисование рыбы
{
    txSetColor(TX_BLACK);
    txSetFillColor(TX_LIGHTBLUE);
    POINT body[9] = {{int(x - 1.5*r/4), int(y - r/8)}, {int(x - r/2  ), int(y - r/2)}, {int(x - r/4    ), int(y - 2.25*r/4)},              //тело
                     {int(x + r/4    ), int(y - r/2)}, {int(x + 3*r/4), int(y - r/4)}, {int(x + 3*r/4  ), int(y           )},
                     {int(x + r/4    ), int(y + r/2)}, {int(x - r/2  ), int(y + r/2)}, {int(x - 1.5*r/4), int(y + r/8     )}};
    txPolygon(body, 9);
    txSetFillColor(RGB(41, 185, 207));
    POINT pl1[6]  = {{int(x - r/2)    , int(y - r/2         )}, {int(x - 1.3*r/4), int(y - 0.75*r)}, {int(x + 1.5*r/4), int(y - 0.75*r  )},    //верхний плавник
                     {int(x + 1.3*r/4), int(y - 1.75*r/4    )}, {int(x + r/4    ), int(y - r/2   )}, {int(x - r/4    ), int(y - 2.25*r/4)}};
    txPolygon(pl1, 6);
    POINT pl2[3]  = {{int(x - r/2  ), int(y + r/2)}, {int(x - r/4), int(y + r/2)}, {int(x - r/4), int(y + 3*r/4)}};                        //нижний передний плавник
    txPolygon(pl2, 3);
    POINT pl3[3]  = {{int(x        ), int(y + r/2)}, {int(x + r/4), int(y + r/2)}, {int(x + r/4), int(y + 3*r/4)}};                        //нижний задний плавник
    txPolygon(pl3, 3);
    POINT tail[4] = {{int(x + 3*r/4), int(y - r/4)}, {int(x + r  ), int(y - r/2)}, {int(x + r  ), int(y + r/4  )},                         //хвост
                     {int(x + 3*r/4), int(y      )}};
    txPolygon(tail, 4);
    txSetFillColor(RGB(227, 197, 145));
    POINT head[7] = {{int(x - 1.5*r/4), int(y - r/8)}, {int(x - r/2  ), int(y - r/2    )}, {int(x - 0.8*r), int(y - 0.3*r)},               //голова
                     {int(x - r      ), int(y + r/8)}, {int(x - 0.8*r), int(y + 1.3*r/4)}, {int(x - r/2  ), int(y + r/2  )},
                     {int(x - 1.5*r/4), int(y + r/8)}};
    txPolygon(head, 7);
    txSetFillColor(TX_BLACK);                              //глаз
    txCircle(int(x - 2.5*r/4), int(y - r/8), 2);
}

void CGrass :: collideWith(shared_ptr<COceanObject> pOther)                                //столкновение травы
{
    auto pFish  = dynamic_pointer_cast <CFish> (pOther);
    if(pFish)
        r = r - 3;
}

void CGrass :: draw() const                                                                //рисование травы
{
    txSetColor(TX_BLACK);
    txSetFillColor(TX_GREEN);
    POINT a[4] = {{x - 10, y}, {x - 10, y - r + 40}, {x + 10, y - r}, {x + 10, y}};
    txPolygon(a, 4);
    POINT b[4] = {{x - 30, y}, {x - 30, y - r + 60}, {x - 10, y - r + 80}, {x - 10, y}};
    txPolygon(b, 4);
    POINT c[4] = {{x + 10, y}, {x + 10, y - r + 80}, {x + 30, y - r + 40}, {x + 30, y}};
    txPolygon(c, 4);
}

void CStone :: draw() const                                                                //рисование камня
{
    txSetColor(TX_BLACK);
    txSetFillColor(TX_GRAY);
    txEllipse(x - r, y - 0.75*r, x + r, y + 0.75*r);
}

