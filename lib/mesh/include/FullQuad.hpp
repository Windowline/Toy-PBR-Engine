#ifndef TOYRENDERER_FULLQUAD_H
#define TOYRENDERER_FULLQUAD_H

class FullQuad {
public:
    FullQuad();
    void render() const;

private:
    unsigned int _VAO;
    unsigned int _VBO;
    unsigned int _EBO;
};


#endif //TOYRENDERER_FULLQUAD_H
