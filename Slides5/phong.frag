varying vec4 myPos;
varying vec4 myNorm;
uniform vec3 lPos;

void main()
{
    vec3 vPos = vec3(myPos.x, myPos.y, myPos.z);

    vec3 normN = vec3(myNorm.x, myNorm.y, myNorm.z);
    normN = normalize(normN);

    vec3 vDir = vec3(0, 0, -1);

    vec3 lDir = normalize(vPos - lPos);

    vec3 Reflection = lDir - 2.0 * dot(lDir, normN) * normN;
    Reflection = normalize(Reflection);

    float intensity = clamp(dot(lDir, normN), 0, 1);

    float keTerm = pow(clamp(dot(Reflection, normalize(vDir)), 0.0, 1.0), 100.0);
    vec4 ksTerm = vec4(0.5, 0.5, 0.5, 0) * keTerm;


    gl_FragColor = 0.2 * gl_Color + gl_Color * intensity + ksTerm;
}
