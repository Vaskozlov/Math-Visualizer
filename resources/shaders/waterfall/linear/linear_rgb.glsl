#version 330 core

vec3 ratioToColor(float ratio) {
    vec3 start_color = vec3(0, 1, 0);
    vec3 end_color = vec3(1, 0, 0);

    return vec3(
        start_color.r + (end_color.r - start_color.r) * ratio,
        start_color.g + (end_color.g - start_color.g) * ratio,
        start_color.b + (end_color.b - start_color.b) * ratio
    );
}

vec3 intToColor(int value, int min_value, int max_value) {
    value = clamp(value, min_value, max_value);
    float ratio = float(value - min_value) / float(max_value - min_value);

    return ratioToColor(ratio);
}

vec3 uintToColor(uint value, uint min_value, uint max_value) {
    value = clamp(value, min_value, max_value);
    float ratio = float(value - min_value) / float(max_value - min_value);

    return ratioToColor(ratio);
}

vec3 floatToColor(float value, float min_value, float max_value) {
    value = clamp(value, min_value, max_value);
    float ratio = (value - min_value) / (max_value - min_value);

    return ratioToColor(ratio);
}
