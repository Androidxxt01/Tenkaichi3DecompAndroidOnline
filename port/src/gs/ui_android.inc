// The in-game menu of the Android build (Dragon Rage), included by ui.cpp in place of the PC's settings window.
// Made for fingers and for a controller: a side bar of pages, large switches, choices as rows of buttons and wide
// sliders, in the launcher's colours (night blue, orange, gold). Only what an Android player can use is here: no
// full screen, display, V-sync or keyboard bindings. Its text is in English, Portuguese or Spanish: the launcher's
// language (setting "dr_lang", written by the app), else the system's.
#include <sys/system_properties.h>

extern "C" int gGsFragDepth; // gs_draw.c: draws may write depth from the fragment shader (precise depth)

static int sLang = -1; // 0 English, 1 Portuguese, 2 Spanish

static int lang(void) {
    if (sLang < 0) {
        int s = Port_Setting("dr_lang", -1);
        sLang = 0;
        if (s >= 0 && s <= 2) {
            sLang = s;
        } else {
            int n = 0;
            SDL_Locale **l = SDL_GetPreferredLocales(&n);
            if (l != NULL && n > 0 && l[0] != NULL && l[0]->language != NULL) {
                if (strncmp(l[0]->language, "pt", 2) == 0) { sLang = 1; }
                else if (strncmp(l[0]->language, "es", 2) == 0) { sLang = 2; }
            }
            SDL_free(l);
        }
    }
    return sLang;
}
#define T(en, pt, es) (lang() == 1 ? (pt) : lang() == 2 ? (es) : (en))

static const ImVec4 kOrange(0.98f, 0.55f, 0.13f, 1.0f), kGold(1.0f, 0.82f, 0.36f, 1.0f), kDim(0.66f, 0.69f, 0.80f, 1.0f);
static const ImVec4 kCard(0.105f, 0.115f, 0.185f, 0.96f), kNight(0.05f, 0.06f, 0.11f, 0.97f);

static float fs(void) { return ImGui::GetFontSize(); }

static void dim_text(const char *text) {
    ImGui::PushStyleColor(ImGuiCol_Text, kDim);
    ImGui::TextWrapped("%s", text);
    ImGui::PopStyleColor();
}

static void card_begin(const char *id, const char *title) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, kCard);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, fs() * 0.7f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(fs() * 1.0f, fs() * 0.8f));
    ImGui::BeginChild(id, ImVec2(0.0f, 0.0f), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_NavFlattened);
    ImGui::PushStyleColor(ImGuiCol_Text, kGold);
    ImGui::TextUnformatted(title);
    ImGui::PopStyleColor();
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + fs() * 2.2f, p.y + fs() * 0.12f), ImGui::GetColorU32(kOrange), fs() * 0.06f);
        ImGui::Dummy(ImVec2(0.0f, fs() * 0.35f));
    }
}

static void card_end(void) {
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0.0f, fs() * 0.4f));
}

// A line of text with an on / off switch at its right; the whole line answers a touch.
static bool aswitch(const char *label, const char *desc, bool *v) {
    ImGui::PushID(label);
    const float w = ImGui::GetContentRegionAvail().x, sw = fs() * 2.6f, sh = fs() * 1.4f;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::BeginGroup();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + w - sw - fs() * 0.8f);
    ImGui::TextUnformatted(label);
    if (desc != NULL && desc[0] != '\0') {
        ImGui::PushStyleColor(ImGuiCol_Text, kDim);
        ImGui::TextWrapped("%s", desc);
        ImGui::PopStyleColor();
    }
    ImGui::PopTextWrapPos();
    ImGui::EndGroup();
    const float h = ImGui::GetItemRectSize().y > sh ? ImGui::GetItemRectSize().y : sh;
    ImGui::SetCursorScreenPos(p);
    bool clicked = ImGui::InvisibleButton("row", ImVec2(w, h));
    if (clicked) { *v = !*v; }
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImVec2 a(p.x + w - sw, p.y + (h - sh) * 0.5f), b(p.x + w, p.y + (h + sh) * 0.5f);
    dl->AddRectFilled(a, b, ImGui::GetColorU32(*v ? kOrange : ImVec4(0.24f, 0.26f, 0.36f, 1.0f)), sh * 0.5f);
    const float r = sh * 0.38f, cx = *v ? b.x - sh * 0.5f : a.x + sh * 0.5f;
    dl->AddCircleFilled(ImVec2(cx, (a.y + b.y) * 0.5f), r, ImGui::GetColorU32(*v ? ImVec4(1.0f, 0.97f, 0.9f, 1.0f) : kDim), 24);
    if (ImGui::IsItemFocused() && ImGui::GetIO().NavVisible) {
        dl->AddRect(ImVec2(p.x - 4.0f, p.y - 4.0f), ImVec2(p.x + w + 4.0f, p.y + h + 4.0f), ImGui::GetColorU32(kGold), fs() * 0.3f, 0, 2.0f);
    }
    ImGui::Dummy(ImVec2(0.0f, fs() * 0.25f));
    ImGui::PopID();
    return clicked;
}

// A title and a row of buttons, one for each value; the one nearest to *v is lit. The row wraps.
static bool achoice(const char *label, const char *desc, int *v, const char *const *names, const int *vals, int n) {
    ImGui::PushID(label);
    ImGui::TextUnformatted(label);
    int best = 0, d = 1 << 30;
    for (int i = 0; i < n; i++) {
        int e = abs(vals[i] - *v);
        if (e < d) { d = e; best = i; }
    }
    bool changed = false;
    const float avail = ImGui::GetContentRegionAvail().x, gap = ImGui::GetStyle().ItemSpacing.x;
    float x = 0.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, fs() * 0.6f);
    for (int i = 0; i < n; i++) {
        float bw = ImGui::CalcTextSize(names[i]).x + fs() * 1.6f;
        if (bw > avail) { bw = avail; }
        if (i > 0 && x + bw <= avail) {
            ImGui::SameLine();
        } else {
            x = 0.0f;
        }
        x += bw + gap;
        bool on = i == best;
        if (on) {
            ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kOrange);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.12f, 0.07f, 0.02f, 1.0f));
        }
        ImGui::PushID(i);
        if (ImGui::Button(names[i], ImVec2(bw, fs() * 2.1f)) && !on) {
            *v = vals[i];
            changed = true;
        }
        ImGui::PopID();
        if (on) { ImGui::PopStyleColor(3); }
    }
    ImGui::PopStyleVar();
    if (desc != NULL && desc[0] != '\0') { dim_text(desc); }
    ImGui::Dummy(ImVec2(0.0f, fs() * 0.25f));
    ImGui::PopID();
    return changed;
}

// A title with the value at its right and a slider the whole width under it.
static bool aslider(const char *label, const char *desc, int *v, int lo, int hi, const char *unit) {
    ImGui::PushID(label);
    char val[32];
    snprintf(val, sizeof(val), "%d%s", *v, unit);
    ImGui::TextUnformatted(label);
    ImGui::SameLine(ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - ImGui::CalcTextSize(val).x - ImGui::GetStyle().ItemSpacing.x);
    ImGui::TextColored(kOrange, "%s", val);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, fs() * 1.4f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(fs() * 0.6f, fs() * 0.45f));
    bool changed = ImGui::SliderInt("##s", v, lo, hi, "", ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopStyleVar(2);
    if (desc != NULL && desc[0] != '\0') { dim_text(desc); }
    ImGui::Dummy(ImVec2(0.0f, fs() * 0.25f));
    ImGui::PopID();
    return changed;
}

static void save_setting(const char *name, int value) {
    Port_SettingSave(name, value);
    Port_SettingsWrite();
}

// ---- the pages

static void page_video(PortVideo &v) {
    card_begin("res", T("Picture", "Imagem", "Imagen"));
    {
        static const int q[] = {4, 5, 6, 7, 8, 10, 12, 16, 20, 24};
        static const char *const names[] = {"1x", "1.25x", "1.5x", "1.75x", "2x", "2.5x", "3x", "4x", "5x", "6x"};
        char desc[200];
        snprintf(desc, sizeof(desc), T("%d x %d. 1x is the PlayStation 2's own. Higher is sharper and heavier for the GPU.",
                                       "%d x %d. 1x é a do PlayStation 2. Mais alta fica mais nítida e pesa mais na GPU.",
                                       "%d x %d. 1x es la del PlayStation 2. Más alta se ve más nítida y pesa más en la GPU."),
                 128 * v.scale4, 112 * v.scale4);
        achoice(T("Internal resolution", "Resolução interna", "Resolución interna"), desc, &v.scale4, names, q, 10);
    }
    {
        static const int vals[] = {0, 1333, 1600, 1778, 2222, 2389};
        const char *names[] = {T("Screen", "Tela", "Pantalla"), "4:3", "16:10", "16:9", "20:9", "21:9"};
        int a = v.aspectMilli;
        if (achoice(T("Aspect ratio", "Proporção", "Relación de aspecto"),
                    T("Screen: the shape of this device's screen, nothing squeezed. Wider than 4:3 shows more of the fight.",
                      "Tela: o formato da tela do aparelho, sem esticar. Mais largo que 4:3 mostra mais da luta.",
                      "Pantalla: la forma de la pantalla del equipo, sin estirar. Más ancho que 4:3 muestra más de la pelea."),
                    &a, names, vals, 6)) {
            v.aspectMilli = a;
        }
    }
    if (!sGL) {
        static const int vals[] = {0, 1, 2, 3, 4};
        const char *names[] = {T("Bilinear", "Bilinear", "Bilineal"), T("Sharp", "Nítido", "Nítido"), "FXAA", "FSR 1", "Snapdragon GSR"};
        achoice(T("Screen filter", "Filtro de tela", "Filtro de pantalla"),
                T("How the picture is scaled to the screen. FSR and GSR sharpen; best below 2x.",
                  "Como a imagem é ampliada para a tela. FSR e GSR deixam mais nítido; melhor abaixo de 2x.",
                  "Cómo se amplía la imagen a la pantalla. FSR y GSR la hacen más nítida; mejor debajo de 2x."),
                &v.filter, names, vals, 5);
    }
    {
        static const int vals[] = {0, 1};
        const char *names[] = {T("Sharp (console)", "Nítido (console)", "Nítido (consola)"), T("Smooth", "Suave", "Suave")};
        achoice(T("2D filtering", "Filtro 2D", "Filtro 2D"),
                T("The HUD, menus and text above 1x.", "HUD, menus e textos acima de 1x.", "HUD, menús y textos encima de 1x."),
                &v.smooth2d, names, vals, 2);
    }
    {
        bool on = v.texPack != 0;
        char label[96];
        snprintf(label, sizeof(label), T("Texture pack (%d textures)", "Pacote de texturas (%d texturas)", "Pack de texturas (%d texturas)"), v.texPackCount);
        if (v.texPackCount > 0) {
            if (aswitch(label, NULL, &on)) { v.texPack = on; }
        } else {
            dim_text(T("Texture pack: none installed (the launcher's Textures page).", "Pacote de texturas: nenhum instalado (página Texturas do app).",
                       "Pack de texturas: ninguno instalado (página Texturas de la app)."));
        }
    }
    card_end();

    card_begin("renderer", T("Renderer", "Renderizador", "Renderizador"));
    {
        static int api = -1;
        if (api < 0) { api = Port_Setting("gpu_api", 0) == 1 ? 1 : 0; }
        static const int vals[] = {0, 1};
        const char *names[] = {"Vulkan", "OpenGL ES"};
        if (achoice(T("Graphics API", "API gráfica", "API gráfica"),
                    T("From the next start of the game. If it cannot start, the other one is tried.",
                      "Vale a partir da próxima vez que o jogo abrir. Se não iniciar, o outro é tentado.",
                      "Vale desde el próximo inicio del juego. Si no arranca, se prueba el otro."),
                    &api, names, vals, 2)) {
            Port_SettingSave("gpu_api", api);
            Port_SettingSave("dr_renderer", api + 1);
            Port_SettingsWrite();
        }
        ImGui::TextColored(kDim, "%s: %s  ·  GPU: %s", T("Running now", "Rodando agora", "Ahora"), sGL ? "OpenGL ES" : "Vulkan", sGpuName);
    }
    card_end();
}

static void page_performance(PortVideo &v) {
    card_begin("speed", T("Speed", "Velocidade", "Velocidad"));
    {
        bool fsk = v.frameSkip != 0;
        if (aswitch(T("Skip pictures instead of slowing down", "Pular quadros em vez de ficar lento", "Saltar cuadros en vez de ir lento"),
                    T("When the GPU is behind, the game keeps its speed and leaves a picture out.",
                      "Quando a GPU não dá conta, o jogo mantém a velocidade e pula um quadro.",
                      "Cuando la GPU no alcanza, el juego mantiene la velocidad y salta un cuadro."), &fsk)) {
            v.frameSkip = fsk;
        }
    }
    {
        bool fd = gGsFragDepth != 0;
        if (aswitch(T("Precise depth", "Profundidade precisa", "Profundidad precisa"),
                    T("Exact overlap of surfaces in one plane (waterfalls, decals). Off is faster on weak GPUs; rare flicker.",
                      "Sobreposição exata de superfícies no mesmo plano (cachoeiras, decalques). Desligado é mais rápido em GPUs fracas; pode piscar raramente.",
                      "Superposición exacta de superficies en el mismo plano (cascadas, calcos). Apagado es más rápido en GPUs débiles; puede parpadear."),
                    &fd)) {
            gGsFragDepth = fd;
            save_setting("frag_depth", fd);
        }
    }
    {
        static const int vals[] = {0, 1, 2};
        const char *names[] = {T("Off (30)", "Desligado (30)", "Apagado (30)"), "60", "120"};
        int smooth = gsInterp < 0 ? 0 : gsInterp > 2 ? 2 : gsInterp;
        if (achoice(T("Smooth motion", "Movimento suave", "Movimiento suave"),
                    T("Extra pictures between the game's 30. Costs GPU; the fight itself does not change.",
                      "Quadros extras entre os 30 do jogo. Pesa na GPU; a luta em si não muda.",
                      "Cuadros extra entre los 30 del juego. Pesa en la GPU; la pelea no cambia."),
                    &smooth, names, vals, 3)) {
            gsInterp = smooth;
            save_setting("interp", gsInterp);
        }
    }
    if (!sGL) {
        static const int vals[] = {0, 1, 2, 3};
        const char *names[] = {T("Automatic", "Automático", "Automático"), "FIFO (VSync)", "Mailbox", "Immediate"};
        int pm = GsGpu_PresentGet();
        if (achoice(T("Present mode", "Modo de apresentação", "Modo de presentación"),
                    T("FIFO: steady, a little more latency. Mailbox: no tearing. Immediate: least latency.",
                      "FIFO: estável, um pouco mais de atraso. Mailbox: sem rasgos. Immediate: menor atraso.",
                      "FIFO: estable, un poco más de retraso. Mailbox: sin cortes. Immediate: menor retraso."),
                    &pm, names, vals, 4)) {
            GsGpu_PresentSet(pm);
            Port_SettingsWrite();
        }
    }
    {
        bool on = meter_on();
        if (aswitch(T("Show frame rate", "Mostrar FPS", "Mostrar FPS"),
                    T("And the connection in an online match.", "E a conexão numa partida online.", "Y la conexión en una partida online."), &on)) {
            sMeter = on;
            save_setting("meter", sMeter);
        }
    }
    card_end();

    card_begin("diag", T("Diagnostics", "Diagnóstico", "Diagnóstico"));
    {
        char model[PROP_VALUE_MAX] = "", soc[PROP_VALUE_MAX] = "";
        __system_property_get("ro.product.model", model);
        __system_property_get("ro.soc.model", soc);
        ImGuiIO &io = ImGui::GetIO();
        ImGui::Text("%s: %.0f fps", T("Pictures", "Quadros", "Cuadros"), io.Framerate);
        ImGui::Text("GPU: %s", sGpuName);
        ImGui::Text("%s: %s", T("Renderer", "Renderizador", "Renderizador"), sGL ? "OpenGL ES 3.2" : "Vulkan");
        ImGui::Text("%s: %gx (%d x %d)", T("Resolution", "Resolução", "Resolución"), v.scale4 / 4.0, 128 * v.scale4, 112 * v.scale4);
        ImGui::Text("%s: %s%s%s", T("Device", "Aparelho", "Equipo"), model, soc[0] ? "  ·  " : "", soc);
        dim_text(T("The full log is in the launcher's Debug page (Export log).", "O log completo fica na página Depuração do app (Exportar log).",
                   "El log completo está en la página Depuración de la app (Exportar log)."));
    }
    card_end();
}

static void page_effects(PortVideo &v) {
    card_begin("fx", T("PlayStation 2 effects", "Efeitos do PlayStation 2", "Efectos del PlayStation 2"));
    const char *names[6] = {T("Outline", "Contorno", "Contorno"), T("See-through tint", "Tom através do cenário", "Tono a través del escenario"),
                            T("Depth tint", "Névoa de distância", "Niebla de distancia"), T("Glare and glow", "Brilho e reflexo", "Brillo y resplandor"),
                            T("Distance blur", "Desfoque de distância", "Desenfoque de distancia"), T("Movement blur", "Desfoque de movimento", "Desenfoque de movimiento")};
    for (int i = 0; i < 6; i++) {
        bool on = !((v.fxOff >> i) & 1);
        if (aswitch(names[i], NULL, &on)) { v.fxOff = (v.fxOff & ~(1 << i)) | (on ? 0 : 1 << i); }
    }
    aslider(T("Glow strength", "Intensidade do brilho", "Intensidad del brillo"),
            T("100% is the PlayStation 2's; 60% by default.", "100% é o do PlayStation 2; 60% por padrão.", "100% es el del PlayStation 2; 60% por defecto."),
            &v.glow, 0, 200, "%");
    card_end();
}

static void page_audio(PortVideo &v) {
    card_begin("vol", T("Volume", "Volume", "Volumen"));
    aslider(T("Music", "Música", "Música"), NULL, &v.music, 0, 200, "%");
    aslider(T("Sound effects", "Efeitos sonoros", "Efectos de sonido"), NULL, &v.effects, 0, 200, "%");
    aslider(T("Voices", "Vozes", "Voces"), NULL, &v.voices, 0, 200, "%");
    aslider(T("Stage sounds", "Sons do cenário", "Sonidos del escenario"), NULL, &v.stage, 0, 200, "%");
    {
        bool follows = gPortMusicFollows > 0;
        if (aswitch(T("Music follows game speed", "Música acompanha a velocidade", "La música sigue la velocidad"),
                    T("Outside 100% speed (online, Cheats page).", "Fora de 100% de velocidade (online, página Trapaças).", "Fuera de 100% de velocidad (online, página Trucos)."),
                    &follows)) {
            gPortMusicFollows = follows;
            save_setting("music_follows_speed", follows);
        }
    }
    card_end();
}

static void page_interface(PortVideo &v) {
    card_begin("hud", T("Screen layout", "Layout da tela", "Diseño de pantalla"));
    {
        static const int vals[] = {0, 1, 2};
        const char *names[] = {T("Edges", "Bordas", "Bordes"), T("Edges, long bars", "Bordas, barras longas", "Bordes, barras largas"), T("Middle (4:3)", "Centro (4:3)", "Centro (4:3)")};
        achoice(T("HUD in a wide picture", "HUD na tela larga", "HUD en pantalla ancha"), NULL, &v.hudLayout, names, vals, 3);
    }
    {
        bool m43 = Port_Menus43() != 0;
        if (aswitch(T("Menus in 4:3", "Menus em 4:3", "Menús en 4:3"),
                    T("Menus at their own shape instead of stretched; fights stay wide.", "Menus no formato original em vez de esticados; lutas continuam largas.",
                      "Menús en su forma original en vez de estirados; las peleas siguen anchas."), &m43)) {
            Port_Menus43Set(m43);
            save_setting("menus_43", m43);
        }
    }
    {
        static const int vals[] = {0, 1, 2};
        const char *names[] = {T("Black", "Preto", "Negro"), T("Border", "Moldura", "Marco"), T("Fighter panels", "Painéis dos lutadores", "Paneles de luchadores")};
        int b = sBars < 0 ? 0 : sBars;
        if (achoice(T("Side bars", "Barras laterais", "Barras laterales"),
                    T("What fills the sides of a 4:3 picture.", "O que preenche as laterais de uma imagem 4:3.", "Lo que llena los lados de una imagen 4:3."),
                    &b, names, vals, 3)) {
            sBars = b;
            save_setting("side_bars", sBars);
        }
    }
    card_end();

    card_begin("fonts", T("Fonts", "Fontes", "Fuentes"));
    struct { const char *label; int *v; int use; } f[3] = {
        {T("Menu font", "Fonte dos menus", "Fuente de menús"), &v.menuFont, MENU_TEXT_MENU},
        {T("Battle font", "Fonte da luta", "Fuente de pelea"), &v.battleFont, MENU_TEXT_BATTLE},
        {T("Dialogue font", "Fonte dos diálogos", "Fuente de diálogos"), &v.textFont, MENU_TEXT_DIALOGUE},
    };
    for (int k = 0; k < 3; k++) {
        const char *names[16];
        int vals[16], n = 0;
        for (int i = 0; i < MenuText_FontCount() && n < 16; i++) {
            if (i == 0 || MenuText_FontAvailable(i, f[k].use)) {
                names[n] = i == 0 ? T("Original", "Original", "Original") : MenuText_FontName(i);
                vals[n++] = i;
            }
        }
        achoice(f[k].label, NULL, f[k].v, names, vals, n);
    }
    card_end();
}

static void page_controls(void) {
    card_begin("clash", T("Clashes", "Choques", "Choques"));
    {
        bool cs = Port_ClashSquare() != 0;
        if (aswitch(T("Punch button counts in clashes", "Botão de soco conta nos choques", "Botón de golpe cuenta en choques"),
                    T("In a clash of beams or rushes each press of Square (X on Xbox) counts as a d-pad tap. Works online.",
                      "Num choque de rajadas ou de golpes, cada toque em Quadrado (X no Xbox) conta como um toque no direcional. Funciona online.",
                      "En un choque de rayos o golpes, cada toque de Cuadrado (X en Xbox) cuenta como un toque de la cruceta. Funciona online."),
                    &cs)) {
            Port_ClashSquareSet(cs);
        }
        if (cs) {
            static const int q[] = {4, 6, 7, 8};
            static const char *const names[] = {"1", "1.5", "1.75", "2"};
            int at = Port_ClashSquareX();
            if (achoice(T("Worth of a press", "Valor de cada toque", "Valor de cada toque"), NULL, &at, names, q, 4)) {
                Port_ClashSquareXSet(at);
            }
        }
    }
    card_end();

    card_begin("pad", T("Physical controller", "Controle físico", "Control físico"));
    {
        int count = 0;
        int *slot = PortInput_PadSlot(0), *pads = PortInput_PadButtons(0);
        SDL_JoystickID *ids = SDL_GetGamepads(&count);
        if (count == 0) {
            dim_text(T("No controller connected. The on-screen controller is set up in the launcher (Controls).",
                       "Nenhum controle conectado. O controle na tela é configurado no app (Controles).",
                       "Ningún control conectado. El control en pantalla se configura en la app (Controles)."));
        } else {
            ImGui::Text("%s: %s", T("Player 1", "Jogador 1", "Jugador 1"), *slot >= 0 && *slot < count ? SDL_GetGamepadNameForID(ids[*slot]) : "-");
        }
        SDL_free(ids);
        if (count > 0) {
            sticks_panel(0);
            ImGui::Spacing();
            dim_text(T("Tap a binding, then press the new button (5 s without a press keeps it).",
                       "Toque numa função e aperte o botão novo (5 s sem apertar mantém).",
                       "Toca una función y pulsa el botón nuevo (5 s sin pulsar la mantiene)."));
            char label[96];
            for (int a = 0; a < PORT_PAD_BUTTONS; a++) {
                bool wait = sCapKind == 2 && sCapPlayer == 0 && sCapAction == a;
                ImGui::PushID(a);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(PortInput_ActionLabel(a));
                ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);
                snprintf(label, sizeof(label), "%s", wait ? T("press a button...", "aperte um botão...", "pulsa un botón...") : pad_source_name(pads[a]));
                if (ImGui::Button(label, ImVec2(-FLT_MIN, fs() * 1.9f))) {
                    capture(2, 0, a);
                }
                ImGui::PopID();
            }
            if (ImGui::Button(T("Reset to defaults", "Restaurar padrão", "Restaurar valores"), ImVec2(-FLT_MIN, fs() * 2.0f))) {
                PortInput_ResetDefaults(0);
                PortInput_Save();
            }
        }
    }
    card_end();
}

static void page_online(void) {
    card_begin("how", T("How online works", "Como o online funciona", "Cómo funciona el online"));
    ImGui::TextWrapped("%s", T(
        "Online play is the PC version's: Dragon Rage on Android and the PC game meet in the same rooms.\n\n"
        "1. In the game's main menu choose Tenkaichi Budokai Online (or Dragon Net Battle).\n"
        "2. Quick match finds an opponent by itself. Create room gives a code; your friend types it in Join.\n"
        "3. Only button presses travel; both games compute the fight, with rollback for late presses.\n"
        "4. Both players need the same release and the same game (the USA disc, SLUS-21678).",
        "O online é o mesmo da versão de PC: o Dragon Rage no Android e o jogo no PC se encontram nas mesmas salas.\n\n"
        "1. No menu principal do jogo escolha Tenkaichi Budokai Online (ou Dragon Net Battle).\n"
        "2. Partida rápida acha um adversário sozinha. Criar sala gera um código; seu amigo digita em Entrar.\n"
        "3. Só os botões apertados viajam; os dois jogos calculam a luta, com rollback para toques atrasados.\n"
        "4. Os dois precisam da mesma versão e do mesmo jogo (o disco americano, SLUS-21678).",
        "El online es el mismo de la versión de PC: Dragon Rage en Android y el juego de PC se encuentran en las mismas salas.\n\n"
        "1. En el menú principal del juego elige Tenkaichi Budokai Online (o Dragon Net Battle).\n"
        "2. Partida rápida busca un rival sola. Crear sala da un código; tu amigo lo escribe en Unirse.\n"
        "3. Solo viajan los botones pulsados; los dos juegos calculan la pelea, con rollback para toques atrasados.\n"
        "4. Los dos necesitan la misma versión y el mismo juego (el disco americano, SLUS-21678)."));
    card_end();

    card_begin("fail", T("If it does not connect", "Se não conectar", "Si no conecta"));
    ImGui::TextWrapped("%s", T(
        "- \"Too many rooms\" or \"busy\": the free matchmaking service has hourly limits. Wait a few minutes.\n"
        "- \"Update available\" or \"different version\": update Dragon Rage (the launcher says when there is one).\n"
        "- \"Room codes need an official release\": the service only takes codes from signed releases; use Quick match or play by address.\n"
        "- Stuck on connecting: try Wi-Fi instead of mobile data (or the other way round), turn off VPN and data saver.\n"
        "- Lag: both on Wi-Fi, near the router; the frame rate meter shows the connection.\n"
        "- Still failing: export the log in the launcher (Debug page) and send it on our Discord.",
        "- \"Too many rooms\" ou \"busy\": o serviço gratuito de partidas tem limite por hora. Espere alguns minutos.\n"
        "- \"Update available\" ou \"different version\": atualize o Dragon Rage (o app avisa quando tem versão nova).\n"
        "- \"Room codes need an official release\": o serviço só aceita códigos de versões assinadas; use Partida rápida ou jogue por endereço (IP).\n"
        "- Travado em conectando: troque Wi-Fi por dados móveis (ou o contrário), desligue VPN e economia de dados.\n"
        "- Lag: os dois no Wi-Fi, perto do roteador; o medidor de FPS mostra a conexão.\n"
        "- Continua falhando: exporte o log no app (página Depuração) e mande no nosso Discord.",
        "- \"Too many rooms\" o \"busy\": el servicio gratuito de partidas tiene límite por hora. Espera unos minutos.\n"
        "- \"Update available\" o \"different version\": actualiza Dragon Rage (la app avisa cuando hay versión nueva).\n"
        "- \"Room codes need an official release\": el servicio solo acepta códigos de versiones firmadas; usa Partida rápida o juega por dirección (IP).\n"
        "- Se queda conectando: cambia Wi-Fi por datos móviles (o al revés), apaga VPN y ahorro de datos.\n"
        "- Lag: los dos en Wi-Fi, cerca del router; el medidor de FPS muestra la conexión.\n"
        "- Sigue fallando: exporta el log en la app (página Depuración) y mándalo en nuestro Discord."));
    card_end();

    card_begin("chat", T("Online chat", "Chat online", "Chat online"));
    {
        bool chat = PortChat_Enabled() != 0;
        if (aswitch(T("Show the other player's messages", "Mostrar mensagens do outro jogador", "Mostrar mensajes del otro jugador"),
                    T("Typing needs a keyboard (Bluetooth or USB): Tab opens a line.", "Para escrever precisa de teclado (Bluetooth ou USB): Tab abre a linha.",
                      "Para escribir hace falta teclado (Bluetooth o USB): Tab abre la línea."), &chat)) {
            PortChat_EnabledSet(chat);
        }
    }
    card_end();
}

static void page_cheats(void) {
    card_begin("spd", T("Game speed", "Velocidade do jogo", "Velocidad del juego"));
    {
        int speed = Port_GameSpeed();
        if (aslider(T("Speed", "Velocidade", "Velocidad"),
                    T("Outside online play only. 100% is the game's own; 114% is what competitive play uses.",
                      "Só fora do online. 100% é a do jogo; 114% é a usada no competitivo.",
                      "Solo fuera del online. 100% es la del juego; 114% es la del competitivo."),
                    &speed, 50, 200, "%")) {
            Port_GameSpeedSet(speed);
            Port_AudioRefresh();
        }
        if (ImGui::Button(T("Back to 100%", "Voltar para 100%", "Volver a 100%"), ImVec2(0.0f, fs() * 2.0f))) {
            Port_GameSpeedSet(100);
            Port_AudioRefresh();
        }
    }
    card_end();

    card_begin("unlock", T("Unlock everything", "Desbloquear tudo", "Desbloquear todo"));
    dim_text(T("All characters, stages, music, items and the most Zenni: a debug function the developers left in. It empties your "
               "records list and becomes permanent when the game saves. Use it from the main menu on.",
               "Todos os personagens, cenários, músicas, itens e o máximo de Zenni: uma função de debug que os desenvolvedores deixaram. "
               "Ela apaga sua lista de recordes e fica permanente quando o jogo salvar. Use a partir do menu principal.",
               "Todos los personajes, escenarios, música, objetos y el máximo de Zenni: una función de depuración que dejaron los desarrolladores. "
               "Vacía tu lista de récords y queda permanente cuando el juego guarda. Úsala desde el menú principal."));
    static bool sure;
    if (!sure) {
        if (ImGui::Button(T("Unlock everything...", "Desbloquear tudo...", "Desbloquear todo..."), ImVec2(-FLT_MIN, fs() * 2.2f))) { sure = true; }
    } else {
        ImGui::TextColored(kGold, "%s", T("Sure? This cannot be undone once the game saves.", "Certeza? Não dá para desfazer depois que o jogo salvar.",
                                          "¿Seguro? No se puede deshacer cuando el juego guarde."));
        if (ImGui::Button(T("Unlock", "Desbloquear", "Desbloquear"), ImVec2(fs() * 8.0f, fs() * 2.2f))) {
            gPortUnlockAll = 1;
            sure = false;
        }
        ImGui::SameLine();
        if (ImGui::Button(T("Cancel", "Cancelar", "Cancelar"), ImVec2(fs() * 8.0f, fs() * 2.2f))) { sure = false; }
    }
    if (gPortUnlockDone) {
        ImGui::TextColored(kOrange, "%s", T("Done.", "Feito.", "Listo."));
    }
    card_end();

    card_begin("start", T("Game start", "Início do jogo", "Inicio del juego"));
    {
        static int logos = -1, intro = -1, copy = -1;
        if (logos < 0) {
            logos = Port_Setting("skip_logos", 0) != 0;
            intro = Port_Setting("skip_intro", 0) != 0;
            copy = Port_Setting("skip_copyright", 0) != 0;
        }
        bool b = logos != 0;
        if (aswitch(T("Skip logos", "Pular logos", "Saltar logos"), NULL, &b)) { logos = b; save_setting("skip_logos", logos); }
        b = intro != 0;
        if (aswitch(T("Skip intro video", "Pular vídeo de abertura", "Saltar video de apertura"), NULL, &b)) { intro = b; save_setting("skip_intro", intro); }
        b = copy != 0;
        if (aswitch(T("Skip copyright page", "Pular tela de copyright", "Saltar pantalla de copyright"), NULL, &b)) { copy = b; save_setting("skip_copyright", copy); }
        dim_text(T("From the next start of the game.", "Vale na próxima vez que o jogo abrir.", "Vale desde el próximo inicio del juego."));
    }
    card_end();
}

// The full-screen window of the Android menus (pause menu, online): night blue with the launcher's glow, text about a
// thirtieth of the screen's height, the launcher's colours. dr_end() closes it.
static void dr_begin(const char *id) {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, kNight);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    const float pad = fs() * 1.1f;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(pad * 1.4f, pad * 0.8f));
    ImGui::Begin(id, NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    // text about a thirtieth of the screen's height (the system's density made it much larger on phones), and the
    // launcher's colours for buttons, fields and sliders
    float base = ImGui::GetStyle().FontSizeBase > 0.0f ? ImGui::GetStyle().FontSizeBase : ImGui::GetFontSize();
    float want = io.DisplaySize.y / 29.0f;
    if (want > ImGui::GetFontSize()) { want = ImGui::GetFontSize(); }
    if (want < 22.0f) { want = 22.0f; }
    ImGui::PushFont(NULL, base * want / ImGui::GetFontSize());
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.18f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.27f, 0.40f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.98f, 0.55f, 0.13f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.18f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.22f, 0.24f, 0.36f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.27f, 0.40f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, kOrange);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, kGold);
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0.98f, 0.55f, 0.13f, 0.45f));
    ImGui::PushStyleColor(ImGuiCol_NavCursor, kGold);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, fs() * 0.45f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(fs() * 0.45f, fs() * 0.4f));
    {   // a warm glow from the top right, as the launcher's background
        ImDrawList *bg = ImGui::GetWindowDrawList();
        const ImVec2 o = ImGui::GetWindowPos(), s = ImGui::GetWindowSize();
        for (int i = 0; i < 6; i++) {
            float r = s.y * (0.35f + 0.13f * i);
            bg->AddCircleFilled(ImVec2(o.x + s.x * 0.92f, o.y - s.y * 0.1f), r, ImGui::GetColorU32(ImVec4(0.98f, 0.55f, 0.13f, 0.035f)), 64);
        }
    }

}

static void dr_end(void) {
    touch_scroll();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(11);
    ImGui::PopFont();
    ImGui::End();
}

static void build_android(void) {
    PortVideo v, was;
    ImGuiIO &io = ImGui::GetIO();
    static int page;
    static bool askQuit;
    bool open = true;

    GsGpu_GetSettings(&v);
    was = v;
    if (sCapKind == 2 && SDL_GetTicks() > sCapUntil) {
        sCapKind = 0;
        gPortInputCapture = 0;
    }
    if (pad_pressed(ImGuiKey_GamepadFaceRight)) { // Circle / B: back to the game
        if (askQuit) { askQuit = false; } else { open = false; }
    }
    const char *pages[] = {
        T("Video", "Vídeo", "Video"), T("Performance", "Desempenho", "Rendimiento"), T("Effects", "Efeitos", "Efectos"),
        T("Audio", "Áudio", "Audio"), T("Interface", "Interface", "Interfaz"), T("Controls", "Controles", "Controles"),
        T("Online", "Online", "Online"), T("Cheats", "Trapaças", "Trucos"),
    };
    const int npages = 8;
    if (pad_pressed(ImGuiKey_GamepadL1)) { page = (page + npages - 1) % npages; }
    if (pad_pressed(ImGuiKey_GamepadR1)) { page = (page + 1) % npages; }

    dr_begin("###DragonRagePause");

    // ---- the top line: name, state, and the two ways out
    {
        const float bh = fs() * 2.4f, right = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;
        ImGui::PushFont(NULL, (ImGui::GetStyle().FontSizeBase > 0.0f ? ImGui::GetStyle().FontSizeBase : ImGui::GetFontSize()) * 1.45f);
        ImGui::TextColored(kGold, "DRAGON RAGE");
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(kDim, "  %s", T("PAUSED", "PAUSADO", "EN PAUSA"));
        const char *resume = T("Resume", "Continuar", "Continuar"), *quit = T("Quit to launcher", "Sair para o app", "Salir a la app");
        const float wr = ImGui::CalcTextSize(resume).x + fs() * 2.6f, wq = ImGui::CalcTextSize(quit).x + fs() * 2.0f;
        ImGui::SameLine(right - wr - wq - ImGui::GetStyle().ItemSpacing.x);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, bh * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.17f, 0.26f, 1.0f));
        if (ImGui::Button(quit, ImVec2(wq, bh))) { askQuit = true; }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.65f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.12f, 0.07f, 0.02f, 1.0f));
        if (ImGui::Button(resume, ImVec2(wr, bh))) { open = false; }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
    }
    ImGui::Dummy(ImVec2(0.0f, fs() * 0.3f));

    if (askQuit) {
        card_begin("quit", T("Quit to the launcher?", "Sair para o app?", "¿Salir a la app?"));
        ImGui::TextWrapped("%s", T("The game closes and you go back to the app. Progress not saved in the game is lost.",
                                   "O jogo fecha e você volta para o app. O progresso que não foi salvo no jogo é perdido.",
                                   "El juego se cierra y vuelves a la app. Se pierde el progreso no guardado en el juego."));
        ImGui::Spacing();
        if (ImGui::Button(T("Quit", "Sair", "Salir"), ImVec2(fs() * 9.0f, fs() * 2.3f))) { quit_to_app(); }
        ImGui::SameLine();
        if (ImGui::Button(T("Stay", "Ficar", "Quedarme"), ImVec2(fs() * 9.0f, fs() * 2.3f))) { askQuit = false; }
        card_end();
    }

    // ---- the side bar of pages and the page
    const float side = io.DisplaySize.x * 0.2f < fs() * 9.0f ? fs() * 9.0f : io.DisplaySize.x * 0.2f;
    const float foot = fs() * 1.6f;
    ImGui::BeginChild("side", ImVec2(side, -foot), ImGuiChildFlags_NavFlattened);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, fs() * 0.6f);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.08f, 0.5f));
    for (int i = 0; i < npages; i++) {
        bool on = i == page;
        ImGui::PushStyleColor(ImGuiCol_Button, on ? ImVec4(0.98f, 0.55f, 0.13f, 0.95f) : ImVec4(0.11f, 0.12f, 0.19f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_Text, on ? ImVec4(0.12f, 0.07f, 0.02f, 1.0f) : ImVec4(0.92f, 0.93f, 0.97f, 1.0f));
        ImGui::PushID(i);
        if (ImGui::Button(pages[i], ImVec2(-FLT_MIN, fs() * 2.2f))) { page = i; }
        ImGui::PopID();
        ImGui::PopStyleColor(2);
    }
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
    ImGui::SameLine(0.0f, fs() * 1.0f);
    ImGui::BeginChild("page", ImVec2(0.0f, -foot), ImGuiChildFlags_NavFlattened);
    switch (page) {
    case 0: page_video(v); break;
    case 1: page_performance(v); break;
    case 2: page_effects(v); break;
    case 3: page_audio(v); break;
    case 4: page_interface(v); break;
    case 5: page_controls(); break;
    case 6: page_online(); break;
    default: page_cheats(); break;
    }
    ImGui::EndChild();
    ImGui::PushStyleColor(ImGuiCol_Text, kDim);
    ImGui::TextUnformatted(T("Back button or START + SELECT: resume   ·   L1 / R1: page",
                             "Botão voltar ou START + SELECT: continuar   ·   L1 / R1: página",
                             "Botón atrás o START + SELECT: continuar   ·   L1 / R1: página"));
    ImGui::PopStyleColor();
    dr_end();
    if (memcmp(&v, &was, sizeof(v)) != 0) {
        GsGpu_SetSettings(&v);
    }
    if (!open) {
        askQuit = false;
        set_open(false);
    }
}

// ---- Dragon Net Battle, the online window opened from the game's main menu, in the same look and for touch. What it
// does is the PC window's (build_net in ui.cpp): the same lobby calls, settings and rules, so that an Android player
// and a PC player meet in the same rooms.

static bool ainput(const char *label, char *buf, size_t size, ImGuiInputTextFlags flags) {
    ImGui::PushID(label);
    ImGui::TextUnformatted(label);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(fs() * 0.7f, fs() * 0.55f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, fs() * 0.5f);
    bool done = ImGui::InputText("##t", buf, size, flags);
    ImGui::PopStyleVar(2);
    ImGui::PopID();
    return done;
}

static bool abutton(const char *label, bool primary, float w) {
    if (primary) {
        ImGui::PushStyleColor(ImGuiCol_Button, kOrange);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.65f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.12f, 0.07f, 0.02f, 1.0f));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, fs() * 1.1f);
    bool r = ImGui::Button(label, ImVec2(w, fs() * 2.3f));
    ImGui::PopStyleVar();
    if (primary) { ImGui::PopStyleColor(3); }
    return r;
}

static void build_net_android(void) {
    static char name[17] = "Player", address[64] = "", port[8] = "7000";
    static int tab = 2, roll = -1, delay = 1, battle = -1, timeLimit = 3, speed = -1;
    static bool useCode = true;
    static char roomCode[8] = "";
    static int lastState = 0;
    static Uint64 since; // a quick search's start (it ends by itself after three minutes)
    static const Uint64 kSearchMs = 180000;
    bool open = true;

    if (pad_pressed(ImGuiKey_GamepadFaceRight)) {
        open = false;
    }
    {
        static bool read;
        if (!read) {
            read = true;
            Port_SettingText("net_name", "Player", name, sizeof(name));
        }
    }
    dr_begin("###DragonNetBattle");

    // ---- the top line
    {
        const float right = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;
        ImGui::PushFont(NULL, (ImGui::GetStyle().FontSizeBase > 0.0f ? ImGui::GetStyle().FontSizeBase : ImGui::GetFontSize()) * 1.45f);
        ImGui::TextColored(kGold, "DRAGON NET BATTLE");
        ImGui::PopFont();
        const char *back = T("Back", "Voltar", "Volver");
        const float wb = ImGui::CalcTextSize(back).x + fs() * 2.4f;
        ImGui::SameLine(right - wb);
        if (abutton(back, false, wb)) { open = false; }
    }
    {
        int min = 0, latest = 0, mine = Port_LobbyRelease(&min, &latest);
        if (min > mine) {
            card_begin("old", T("Update needed", "Atualização necessária", "Actualización necesaria"));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.4f, 1.0f));
            ImGui::TextWrapped(T("The online service now asks for the PC release 0.%d.%d; this game is 0.%d.%d. Update Dragon Rage (the app says when "
                                 "there is a new version) to play online again.",
                                 "O serviço online agora pede a versão 0.%d.%d do PC; este jogo é 0.%d.%d. Atualize o Dragon Rage (o app avisa quando "
                                 "tem versão nova) para voltar a jogar online.",
                                 "El servicio online ahora pide la versión 0.%d.%d del PC; este juego es 0.%d.%d. Actualiza Dragon Rage (la app avisa "
                                 "cuando hay versión nueva) para volver a jugar online."),
                               min / 100, min % 100, mine / 100, mine % 100);
            ImGui::PopStyleColor();
            card_end();
        }
    }

    // ---- name and the three ways to play
    {
        const float w = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(w * 0.4f);
        if (ainput(T("Your name", "Seu nome", "Tu nombre"), name, sizeof(name), 0)) {}
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            Port_SettingTextSave("net_name", name);
            Port_SettingsWrite();
        }
        ImGui::PopItemWidth();
    }
    ImGui::Dummy(ImVec2(0.0f, fs() * 0.3f));
    {
        int state0 = Port_LobbyPoll();
        bool busy = state0 == 1 || state0 == 3;
        static const int vals[] = {2, 0, 1};
        const char *names[] = {T("Quick match", "Partida rápida", "Partida rápida"), T("Host", "Criar sala", "Crear sala"), T("Join", "Entrar", "Unirse")};
        ImGui::BeginDisabled(busy);
        achoice(T("Play", "Jogar", "Jugar"), NULL, &tab, names, vals, 3);
        ImGui::EndDisabled();
    }

    if (tab == 0) {
        card_begin("host", T("Host a match", "Criar uma sala", "Crear una sala"));
        aswitch(T("Use a room code", "Usar código de sala", "Usar código de sala"),
                T("You get a short code for the other player; the games find each other through the matchmaking service (no port forwarding). "
                  "Off: host on a port, which the other player must be able to reach.",
                  "Você recebe um código curto para passar ao outro jogador; os jogos se acham pelo serviço de partidas (sem abrir portas). "
                  "Desligado: a sala fica numa porta, que o outro jogador precisa alcançar.",
                  "Recibes un código corto para el otro jugador; los juegos se encuentran por el servicio de partidas (sin abrir puertos). "
                  "Apagado: la sala queda en un puerto que el otro jugador debe alcanzar."), &useCode);
        if (!useCode) { ainput(T("Port", "Porta", "Puerto"), port, sizeof(port), ImGuiInputTextFlags_CharsDecimal); }
        if (roll < 0) {
            roll = Port_RollCan() ? Port_Setting("net_rollback2", 8) / 2 : 0;
            delay = Port_Setting("net_delay2", -1) + 1;
            if (roll < 0 || roll > 4) { roll = 4; }
            if (delay < 0 || delay > 7) { delay = 0; }
        }
        if (Port_RollCan()) {
            static const int rv[] = {0, 1, 2, 3, 4};
            const char *rn[] = {T("Off", "Desligado", "Apagado"), "2", "4", "6", "8"};
            achoice(T("Rollback (frames)", "Rollback (quadros)", "Rollback (cuadros)"),
                    T("Your moves come out at once and the game corrects itself when the other player's buttons arrive.",
                      "Seus golpes saem na hora e o jogo se corrige quando os botões do outro chegam.",
                      "Tus golpes salen al instante y el juego se corrige cuando llegan los botones del otro."), &roll, rn, rv, 5);
        }
        {
            static const int dv[] = {0, 1, 2, 3, 4, 5};
            const char *dn[] = {T("Automatic", "Automático", "Automático"), "0", "1", "2", "3", "4"};
            achoice(T("Input delay (frames)", "Atraso de entrada (quadros)", "Retraso de entrada (cuadros)"), NULL, &delay, dn, dv, 6);
        }
        if (battle < 0) {
            battle = Port_Setting("net_battle", 0);
            timeLimit = Port_Setting("net_time", 3);
            if (battle < 0 || battle > 4) { battle = 0; }
            if (timeLimit < 0 || timeLimit > 4) { timeLimit = 3; }
        }
        {
            static const int bv[] = {0, 1, 2, 3, 4};
            const char *bn[] = {T("Single", "Individual", "Individual"), T("Team", "Equipe", "Equipo"), "DP 10", "DP 15", "DP 20"};
            achoice(T("Battle", "Batalha", "Batalla"), NULL, &battle, bn, bv, 5);
            const char *tn[] = {"60 s", "90 s", "180 s", "240 s", T("No limit", "Sem limite", "Sin límite")};
            achoice(T("Time limit", "Tempo", "Tiempo"), NULL, &timeLimit, tn, bv, 5);
        }
        if (speed < 0) { speed = Port_Setting("net_speed_host", Port_NetSpeedStandard()); }
        {
            char d[160];
            snprintf(d, sizeof(d), T("For both players. 100%% is the game's own; %d%% is the competitive standard.",
                                     "Para os dois jogadores. 100%% é a do jogo; %d%% é o padrão competitivo.",
                                     "Para los dos jugadores. 100%% es la del juego; %d%% es el estándar competitivo."), Port_NetSpeedStandard());
            aslider(T("Game speed", "Velocidade do jogo", "Velocidad del juego"), d, &speed, 100, 150, "%");
        }
        card_end();
    } else if (tab == 2) {
        static Uint64 asked;
        if (sQueue < 0) {
            sQueue = Port_Setting("net_queue", 0);
            if (sQueue < 0 || sQueue > 4) { sQueue = 0; }
        }
        if (!sNetBusy && (asked == 0 || SDL_GetTicks() - asked > 30000)) {
            asked = SDL_GetTicks() + 1;
            Port_LobbyQueuesAsk();
        }
        card_begin("quick", T("Quick match", "Partida rápida", "Partida rápida"));
        char d[200];
        snprintf(d, sizeof(d), T("Matched with whoever searches the same queue, PC or Android. No time limit, %d%% speed.",
                                 "Contra quem estiver procurando na mesma fila, PC ou Android. Sem limite de tempo, velocidade %d%%.",
                                 "Contra quien busque en la misma cola, PC o Android. Sin límite de tiempo, velocidad %d%%."), Port_NetSpeedStandard());
        dim_text(d);
        static const int qv[] = {0, 1, 2, 3, 4};
        char qn[5][64];
        const char *base[5] = {T("Single", "Individual", "Individual"), T("Team", "Equipe", "Equipo"), "DP 10", "DP 15", "DP 20"};
        const char *qp[5];
        for (int i = 0; i < 5; i++) {
            int n = Port_LobbyQueueWaiting(i);
            if (n > 0) { snprintf(qn[i], sizeof(qn[i]), "%s (%d)", base[i], n); } else { snprintf(qn[i], sizeof(qn[i]), "%s", base[i]); }
            qp[i] = qn[i];
        }
        ImGui::BeginDisabled(sNetBusy);
        achoice(T("Queue (players searching)", "Fila (jogadores procurando)", "Cola (jugadores buscando)"), NULL, &sQueue, qp, qv, 5);
        ImGui::EndDisabled();
        card_end();
    } else {
        card_begin("join", T("Join a match", "Entrar numa sala", "Unirse a una sala"));
        ainput(T("Room code", "Código da sala", "Código de sala"), roomCode, sizeof(roomCode), ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank);
        if (abutton(T("Paste the code", "Colar o código", "Pegar el código"), false, fs() * 9.0f)) {
            char *clip = SDL_GetClipboardText();
            size_t n = 0;
            for (const char *c = clip; c != NULL && *c != '\0' && n + 1 < sizeof(roomCode); c++) {
                if ((*c >= '0' && *c <= '9') || (*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z')) {
                    roomCode[n++] = (char)(*c >= 'a' ? *c - 32 : *c);
                }
            }
            roomCode[n] = '\0';
            SDL_free(clip);
        }
        dim_text(T("Or, without a code, the host's address and port:", "Ou, sem código, o endereço e a porta de quem criou:",
                   "O, sin código, la dirección y el puerto de quien la creó:"));
        ImGui::BeginDisabled(roomCode[0] != '\0');
        ainput(T("Host's address", "Endereço do host", "Dirección del host"), address, sizeof(address), 0);
        ainput(T("Port", "Porta", "Puerto"), port, sizeof(port), ImGuiInputTextFlags_CharsDecimal);
        ImGui::EndDisabled();
        card_end();
    }

    // ---- the state of the lobby and the button
    card_begin("state", T("Match", "Partida", "Partida"));
    {
        int state = Port_LobbyPoll();
        sNetBusy = state == 1 || state == 3;
        if (state == 2) {
            Port_LobbyLaunch();
            set_open(false);
            sNet = false;
        }
        bool byCode = tab == 0 ? useCode : roomCode[0] != '\0';
        if (state != 0) { lastState = state; }
        if (tab == 2 && state == 3 && since != 0 && SDL_GetTicks() - since > kSearchMs) {
            Port_LobbyCancel();
            lastState = -5;
            state = 0;
        }
        if (state == 3 || state == 1) {
            if (tab == 2) {
                ImGui::TextWrapped("%s", state == 1 ? T("Found a player. Connecting...", "Achamos um jogador. Conectando...", "Encontramos un jugador. Conectando...")
                                                    : T("Searching this queue...", "Procurando nesta fila...", "Buscando en esta cola..."));
                if (state == 3 && since != 0) {
                    Uint64 left = kSearchMs - (SDL_GetTicks() - since);
                    ImGui::TextColored(kDim, "%d:%02d", (int)(left / 60000), (int)(left / 1000 % 60));
                }
            } else if (byCode && tab == 0 && Port_LobbyCode()[0] != '\0') {
                ImGui::TextUnformatted(T("Room code:", "Código da sala:", "Código de sala:"));
                ImGui::PushFont(NULL, (ImGui::GetStyle().FontSizeBase > 0.0f ? ImGui::GetStyle().FontSizeBase : ImGui::GetFontSize()) * 2.2f);
                ImGui::TextColored(kGold, "%s", Port_LobbyCode());
                ImGui::PopFont();
                static Uint64 copiedAt;
                if (abutton(T("Copy", "Copiar", "Copiar"), false, fs() * 7.0f)) {
                    copiedAt = SDL_SetClipboardText(Port_LobbyCode()) ? SDL_GetTicks() : 0;
                }
                if (copiedAt != 0 && SDL_GetTicks() - copiedAt < 2000) {
                    ImGui::SameLine();
                    ImGui::TextColored(kDim, "%s", T("Copied", "Copiado", "Copiado"));
                }
                ImGui::TextWrapped("%s", state == 3 ? T("Tell the other player this code. Waiting for them...", "Passe este código para o outro jogador. Esperando...",
                                                        "Pásale este código al otro jugador. Esperando...")
                                                    : T("The other player joined. Connecting...", "O outro jogador entrou. Conectando...", "El otro jugador entró. Conectando..."));
            } else if (byCode) {
                ImGui::TextWrapped("%s", state == 1 ? T("Found the room. Connecting...", "Sala encontrada. Conectando...", "Sala encontrada. Conectando...")
                                                    : tab == 0 ? T("Getting a room code...", "Pegando um código...", "Obteniendo un código...")
                                                               : T("Looking for the room...", "Procurando a sala...", "Buscando la sala..."));
            } else {
                ImGui::TextWrapped(tab == 0 ? T("Waiting for the other player on port %s...", "Esperando o outro jogador na porta %s...", "Esperando al otro jugador en el puerto %s...")
                                            : T("Looking for %s...", "Procurando %s...", "Buscando %s..."), tab == 0 ? port : address);
                if (tab == 0 && Port_LobbyOther()) {
                    ImGui::TextWrapped("%s", T("A player with a different version is trying to join: both need the same release.",
                                               "Um jogador com outra versão está tentando entrar: os dois precisam da mesma versão.",
                                               "Un jugador con otra versión intenta entrar: los dos necesitan la misma versión."));
                }
            }
            if (abutton(T("Cancel", "Cancelar", "Cancelar"), false, fs() * 8.0f)) {
                Port_LobbyCancel();
                lastState = 0;
            }
        } else {
            if (lastState == -3) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.4f, 1.0f));
                ImGui::TextWrapped("%s", Port_LobbyError());
                ImGui::PopStyleColor();
            } else if (lastState == -4) {
                ImGui::TextWrapped("%s", T("The two games could not reach each other, directly or through the relay. Try again, or another network.",
                                           "Os dois jogos não conseguiram se alcançar, nem direto nem pelo relay. Tente de novo ou outra rede.",
                                           "Los dos juegos no pudieron alcanzarse, ni directo ni por el relay. Inténtalo de nuevo u otra red."));
            } else if (lastState == -5) {
                ImGui::TextWrapped("%s", T("Nobody searched this queue in the last three minutes. Search again or try another queue.",
                                           "Ninguém procurou nesta fila nos últimos três minutos. Procure de novo ou tente outra fila.",
                                           "Nadie buscó en esta cola en los últimos tres minutos. Busca de nuevo o prueba otra cola."));
            } else if (lastState == -2 || state == -2) {
                ImGui::TextWrapped("%s", T("The other game is a different version: both players need the same release.",
                                           "O outro jogo é de outra versão: os dois precisam da mesma versão.",
                                           "El otro juego es de otra versión: los dos necesitan la misma versión."));
            } else if (state < 0) {
                ImGui::TextWrapped("%s", tab == 0 ? T("That port cannot be used.", "Essa porta não pode ser usada.", "Ese puerto no se puede usar.")
                                                  : T("That address is not known.", "Esse endereço não foi encontrado.", "Esa dirección no se encontró."));
            }
            bool can = tab == 2 || (byCode ? (tab == 0 || strlen(roomCode) == 6) : atoi(port) > 0 && (tab == 0 || address[0] != '\0'));
            ImGui::BeginDisabled(!can);
            const char *go = tab == 2 ? (lastState == -5 ? T("Search again", "Procurar de novo", "Buscar de nuevo") : T("Search for a match", "Procurar partida", "Buscar partida"))
                           : tab == 0 ? T("Host the match", "Criar a sala", "Crear la sala") : T("Join the match", "Entrar na sala", "Unirse a la sala");
            if (abutton(go, true, -FLT_MIN)) {
                Port_NetNameSet(name);
                Port_SettingTextSave("net_name", name);
                lastState = 0;
                if (tab == 2) {
                    since = SDL_GetTicks() + 1;
                    Port_NetOptions(Port_RollCan() ? 8 : 0, -2);
                    Port_NetSpeedSet(Port_NetSpeedStandard());
                    Port_NetRulesSet(sQueue == 0 ? 0 : sQueue == 1 ? 1 : 2, sQueue >= 2 ? sQueue - 2 : 0, 4);
                    Port_SettingSave("net_queue", sQueue);
                    Port_SettingsWrite();
                    Port_LobbyStartQuick(sQueue);
                } else {
                    if (tab == 0) {
                        Port_NetOptions(roll * 2, delay == 0 ? -2 : delay - 1);
                        Port_NetSpeedSet(speed);
                        Port_SettingSave("net_speed_host", speed);
                        Port_NetRulesSet(battle == 0 ? 0 : battle == 1 ? 1 : 2, battle >= 2 ? battle - 2 : 0, timeLimit);
                        Port_SettingSave("net_battle", battle);
                        Port_SettingSave("net_time", timeLimit);
                        Port_SettingSave("net_rollback2", roll * 2);
                        Port_SettingSave("net_delay2", delay - 1);
                    } else {
                        Port_NetOptions(-1, -1);
                    }
                    Port_SettingsWrite();
                    if (byCode) {
                        Port_LobbyStartCode(tab == 0, roomCode);
                    } else {
                        Port_LobbyStart(tab == 0, address, atoi(port));
                    }
                }
            }
            ImGui::EndDisabled();
        }
    }
    card_end();
    dim_text(T("Connected, the game switches to the match: both players get the same roster with everything unlocked, and your own save "
               "is left alone. Leaving the match brings you back here.",
               "Conectado, o jogo vai para a partida: os dois recebem o mesmo elenco com tudo liberado, e o seu save não é mexido. "
               "Saindo da partida você volta para cá.",
               "Conectado, el juego pasa a la partida: los dos reciben el mismo elenco con todo desbloqueado y tu partida guardada no se toca. "
               "Al salir de la partida vuelves aquí."));
    dr_end();
    if (!open) {
        Port_LobbyCancel();
        set_open(false);
    }
}
