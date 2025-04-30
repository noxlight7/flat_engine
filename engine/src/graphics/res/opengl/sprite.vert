#version 460 core

// Атрибуты базового квадрата
layout (location = 0) in vec2 aPos;       // Позиция в объектном пространстве (квадрат с центром в (0,0))
layout (location = 1) in vec2 aTexCoord;    // Текстурные координаты

// Инстанс-атрибуты
layout (location = 2) in vec2 instancePos;      // Позиция центра спрайта в мировом пространстве
layout (location = 3) in vec2 instanceSize;     // Размер спрайта (масштаб)
layout (location = 4) in float instanceRotation; // Угол поворота (в градусах)

out vec2 TexCoord;

uniform mat4 u_projection;
uniform mat4 u_view;

void main() {
    // Приводим угол в радианы
    float rad = radians(instanceRotation);
    // Матрица поворота 2x2 (так как у нас 2D)
    mat2 rot = mat2(cos(rad), -sin(rad),
                    sin(rad),  cos(rad));

    // aPos задан в диапазоне [-0.5, 0.5] для корректного вращения вокруг центра
    vec2 scaledPos = aPos * instanceSize;   // масштабирование
    vec2 rotatedPos = rot * scaledPos;        // поворот
    vec2 worldPos = instancePos + rotatedPos; // перенос в мировые координаты

    gl_Position = u_projection * u_view * vec4(worldPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
