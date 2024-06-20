#ifndef TOYRENDERER_TRIANGLE_HPP
#define TOYRENDERER_TRIANGLE_HPP

class Triangle {
public:
    Triangle();
    void render() const;
private:
    unsigned int _VAO;
    unsigned int _VBO;
    unsigned int _EBO;
};


#endif //TOYRENDERER_TRIANGLE_HPP
