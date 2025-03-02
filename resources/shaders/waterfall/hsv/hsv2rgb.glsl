#version 330 core

vec3 hsv2rgb(vec3 hsvColor)
{
    vec3 hsv = hsvColor;
    vec3 rgbColor;

    if (abs(hsv.y) < 1e-8) {
        rgbColor = vec3(hsv.z);
    }
    else
    {
        float sector = floor(hsv.x * (1.0F / 60.0F));
        float frac = (hsv.x * (1.0F / 60.0F)) - sector;
        // factorial part of h
        float o = hsv.z * (1.0F - hsv.y);
        float p = hsv.z * (1.0F - hsv.y * frac);
        float q = hsv.z * (1.0F - hsv.y * (1.0F - frac));

        switch (int(sector))
        {
            default :
            case 0:
            rgbColor.r = hsv.z;
            rgbColor.g = q;
            rgbColor.b = o;
            break;

            case 1:
            rgbColor.r = p;
            rgbColor.g = hsv.z;
            rgbColor.b = o;
            break;

            case 2:
            rgbColor.r = o;
            rgbColor.g = hsv.z;
            rgbColor.b = q;
            break;

            case 3:
            rgbColor.r = o;
            rgbColor.g = p;
            rgbColor.b = hsv.z;
            break;

            case 4:
            rgbColor.r = q;
            rgbColor.g = o;
            rgbColor.b = hsv.z;
            break;

            case 5:
            rgbColor.r = hsv.z;
            rgbColor.g = o;
            rgbColor.b = p;
            break;
        }
    }

    return rgbColor;
}

vec3 hsvIntToRgb(int value, int min_value, int max_value) {
    float v = clamp(value, min_value, max_value);
    float hue = 360.0F * (v - min_value) / (max_value - min_value);

    return hsv2rgb(vec3(hue, 1.0F, 1.0F));
}

vec3 hsvUintToRgb(uint value, uint min_value, uint max_value) {
    float v = clamp(value, min_value, max_value);
    float hue = 360.0F * (v - min_value) / (max_value - min_value);

    return hsv2rgb(vec3(hue, 1.0F, 1.0F));
}

vec3 hsvFloatToRgb(float value, float min_value, float max_value) {
    float v = clamp(value, min_value, max_value);
    float hue = 360.0F * (v - min_value) / (max_value - min_value);

    return hsv2rgb(vec3(hue, 1.0F, 1.0F));
}
