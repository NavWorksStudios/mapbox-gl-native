

#define MAX_SIZE        4
#define MAX_KERNEL_SIZE ((MAX_SIZE * 2 + 1) * (MAX_SIZE * 2 + 1))
#define MAX_BINS_SIZE   10

uniform sampler2D ssao;

const vec2 parameters = vec2(1, 0);


void main() {
  vec2 texSize  = vec2(2048., 1080.) * 2.;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  int size = int(parameters.x);
  if (size <= 0) {
    gl_FragColor = texture2D(ssao, texCoord);
    return;
  }
  if (size > MAX_SIZE) { size = MAX_SIZE; }
  int kernelSize = int(pow(float(size) * 2. + 1., 2.));

  int binsSize = int(clamp(parameters.y, 1., float(MAX_BINS_SIZE)));

  int i        = 0;
  int j        = 0;
  int count    = 0;
  int binIndex = 0;

  vec4  colors[MAX_KERNEL_SIZE];
  float bins[MAX_BINS_SIZE];
  int   binIndexes[MAX_KERNEL_SIZE];

  float total = 0.;
  float limit = floor(float(kernelSize) / 2.) + 1.;

  float value = 0.;
  vec3  valueRatios = vec3(0.3, 0.59, 0.11);

  for (i = -size; i <= size; ++i) {
    for (j = -size; j <= size; ++j) {
      colors[count] = texture2D(ssao, texCoord + vec2(i,j) / texSize);
      count += 1;
    }
  }

  for (i = 0; i < binsSize; ++i) {
    bins[i] = 0.;
  }

  for (i = 0; i < kernelSize; ++i) {
    value           = dot(colors[i].rgb, valueRatios);
    binIndex        = int(floor(value * float(binsSize)));
    binIndex        = int(clamp(float(binIndex), 0., float(binsSize) - 1.));
    bins[binIndex] += 1.;
    binIndexes[i]   = binIndex;
  }

  binIndex = 0;

  for (i = 0; i < binsSize; ++i) {
    total += bins[i];
    if (total >= limit) {
      binIndex = i;
      break;
    }
  }

  gl_FragColor = colors[0];

  for (i = 0; i < kernelSize; ++i) {
    if (binIndexes[i] == binIndex) {
      gl_FragColor = vec4(colors[i].r);
      break;
    }
  }
}
