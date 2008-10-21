#include "viewer_fni.h"

int fni::load(plString filename) {
    hsStream* S;
    if (plEncryptedStream::IsFileEncrypted(filename)) {
        S = new plEncryptedStream();
        if (!((plEncryptedStream*)S)->open(filename, fmRead, plEncryptedStream::kEncAuto)) {
            return 0;
        }
    }
    else {
        S = new hsFileStream();
        if (!((hsFileStream*)S)->open(filename, fmRead)) {
            return 0;
        }
    }
    while (!S->eof()) {
		plString ln = S->readLine();
        std::vector<plString> splitline = ln.split(' ');
		if (splitline.size() == 0) continue;
		plString var = splitline[0];
        if (var == "Graphics.Renderer.Fog.SetDefColor") {
            this->fDefColor[0] = splitline[1].toFloat();
            this->fDefColor[1] = splitline[2].toFloat();
            this->fDefColor[2] = splitline[3].toFloat();
			printf("1");
        }
        if (var == "Graphics.Renderer.SetClearColor") {
            this->fClearColor[0] = splitline[1].toFloat();
            this->fClearColor[1] = splitline[2].toFloat();
            this->fClearColor[2] = splitline[3].toFloat();
			printf("2");
		}

        if (var == "Graphics.Renderer.Fog.SetDefLinear") {
			this->fFogType = 1;
            this->fDefLinear[0] = splitline[1].toFloat();
            this->fDefLinear[1] = splitline[2].toFloat();
            this->fDefLinear[2] = splitline[3].toFloat();
			printf("3");
		}
        if (var == "Graphics.Renderer.Fog.SetDefExp2") {
			this->fFogType = 3;
			this->fDefExp2[0] = splitline[1].toFloat();
            this->fDefExp2[1] = splitline[2].toFloat();
			printf("4");
		}
        if (var == "Graphics.Renderer.SetYon") {
            this->fYon = splitline[1].toFloat();
		}
    }
    delete S;
    return 1;
}

