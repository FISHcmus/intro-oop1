#include "UIComponents.h"
#include "Fonts.h"
#include "Theme.h"

#include <string>

namespace {
constexpr float kFrameInset = 4.0f;
constexpr float kCapRest    = 14.0f;
constexpr float kCapFocus   = 18.0f;
constexpr float kPressedDx  = 2.0f;
constexpr float kBtnTextSize = 22.0f;
constexpr float kTitleSize   = 50.0f;
constexpr float kHintSize    = 14.0f;
constexpr float kPanelTextSize = 18.0f;

// ASCII avatars. Leading whitespace pre-baked so the face sits under the
// dialogue-bubble tail. Width-consistent across rows so columns stack.
constexpr const char* kAvatarCoSuTien =
    "              . v . v .\n"
    "             | M M M M |\n"
    "              '-------'\n"
    "              ( o   o )\n"
    "               \\  ~  /\n"
    "                '---'\n"
    "              /|     |\\\n"
    "               CÔ SỬ TIÊN";

constexpr const char* kAvatarSonTinh =
    "               _______\n"
    "              |  ===  |\n"
    "              |-     -|\n"
    "              | () () |\n"
    "              |   |   |\n"
    "               \\ --- /\n"
    "                )mmm(\n"
    "                SƠN TINH";

constexpr const char* kAvatarThuyTinh =
    "              \\/\\/\\/\\/\\/\n"
    "              .---------.\n"
    "              |\\       /|\n"
    "              | >     < |\n"
    "               \\   V   /\n"
    "                >wWw<\n"
    "                 '-'\n"
    "               THUY TINH";

const char* avatarFor(UIC::Speaker s) {
    switch (s) {
        case UIC::Speaker::SonTinh:  return kAvatarSonTinh;
        case UIC::Speaker::ThuyTinh: return kAvatarThuyTinh;
        case UIC::Speaker::CoSuTien: return kAvatarCoSuTien;
    }
    return kAvatarCoSuTien;
}

// Codepoint count of a UTF-8 string. Vietnamese diacritics + box-drawing
// chars are multi-byte in UTF-8 but one codepoint each, which is what the
// mono grid sees.
int cpCount(const char* s) {
    int n = 0;
    while (*s) {
        int sz = 0;
        GetCodepoint(s, &sz);
        if (sz <= 0) sz = 1;
        s += sz;
        n++;
    }
    return n;
}

void appendSpaces(std::string& dst, int n) {
    if (n > 0) dst.append(static_cast<size_t>(n), ' ');
}

std::string repeatGlyph(const char* g, int n) {
    std::string s;
    s.reserve(static_cast<size_t>(n) * 3u);
    for (int i = 0; i < n; ++i) s += g;
    return s;
}

// Word-wrap body to <= widthCh columns. Splits on ' ' and honors explicit
// '\n'. Codepoint-aware so multi-byte UTF-8 doesn't overrun.
std::string wrapBody(const char* body, int widthCh) {
    std::string out;
    std::string line;
    int lineLen = 0;
    const char* p = body;
    while (*p) {
        std::string word;
        int wordLen = 0;
        while (*p && *p != ' ' && *p != '\n') {
            int sz = 0;
            GetCodepoint(p, &sz);
            if (sz <= 0) sz = 1;
            word.append(p, static_cast<size_t>(sz));
            p += sz;
            wordLen++;
        }
        if (wordLen > 0) {
            if (lineLen == 0) {
                line = word;
                lineLen = wordLen;
            } else if (lineLen + 1 + wordLen <= widthCh) {
                line += ' ';
                line += word;
                lineLen += 1 + wordLen;
            } else {
                out += line; out += '\n';
                line = word;
                lineLen = wordLen;
            }
        }
        if (*p == ' ') {
            ++p;
        } else if (*p == '\n') {
            out += line; out += '\n';
            line.clear();
            lineLen = 0;
            ++p;
        }
    }
    if (lineLen > 0) out += line;
    return out;
}

// "<edge><content padded to W-2 cols><edge>". edge = ║ for comic frame,
// │ for rounded dialogue bubble.
std::string paddedRow(const std::string& content, int W, const char* edge) {
    int innerW = W - 2;
    int len = cpCount(content.c_str());
    std::string r = edge;
    r += content;
    appendSpaces(r, innerW - len);
    r += edge;
    return r;
}

// Append wrapped body (newline-separated) into out, one paddedRow per line.
void renderWrappedBody(std::string& out, const std::string& wrapped,
                       int W, const char* edge) {
    size_t i = 0;
    while (i <= wrapped.size()) {
        size_t nl = wrapped.find('\n', i);
        std::string line = (nl == std::string::npos)
            ? wrapped.substr(i)
            : wrapped.substr(i, nl - i);
        out += paddedRow(std::string("  ") + line, W, edge);
        out += '\n';
        if (nl == std::string::npos) break;
        i = nl + 1;
    }
}
}  // namespace

namespace UIC {

void drawPrimaryButton(Rectangle r, const char* label, State s) {
    Color frameColor = Theme::palette.ink_sumi;
    Color bodyColor  = Theme::palette.slate_fog;
    Color textColor  = Theme::palette.son_bone;
    Color sonColor   = Theme::palette.son_jade;
    Color thuyColor  = Theme::palette.thuy_cyan;
    float capSize    = kCapRest;
    float ox         = 0.0f;
    float oy         = 0.0f;

    const bool awake = (s == State::Focused || s == State::Pressed);
    if (awake) {
        frameColor = Theme::palette.gold_foil;
        bodyColor  = Theme::palette.ink_sumi;
        textColor  = Theme::palette.gold_foil;
        capSize    = kCapFocus;
        if (s == State::Pressed) {
            ox = kPressedDx;
            oy = kPressedDx;
        }
    } else if (s == State::Disabled) {
        // 90/120/200 alpha keeps disabled reading as part of the frame system.
        sonColor   = Theme::withAlpha(sonColor,  90);
        thuyColor  = Theme::withAlpha(thuyColor, 90);
        textColor  = Theme::withAlpha(textColor, 120);
        bodyColor  = Theme::withAlpha(bodyColor, 200);
    } else {
        // Caps at half-strength so awake state pops by contrast.
        sonColor  = Theme::withAlpha(sonColor,  140);
        thuyColor = Theme::withAlpha(thuyColor, 140);
    }

    Rectangle outer = { r.x + ox, r.y + oy, r.width, r.height };
    DrawRectangleRec(outer, frameColor);

    Rectangle inner = {
        outer.x + kFrameInset, outer.y + kFrameInset,
        outer.width  - 2.0f * kFrameInset,
        outer.height - 2.0f * kFrameInset
    };
    DrawRectangleRec(inner, bodyColor);

    const float ir = inner.x + inner.width;
    const float ib = inner.y + inner.height;

    // Sơn — mountain triangle, top-left.
    Vector2 sonV1 = { inner.x,           inner.y };
    Vector2 sonV2 = { inner.x,           inner.y + capSize };
    Vector2 sonV3 = { inner.x + capSize, inner.y };
    DrawTriangle(sonV1, sonV2, sonV3, sonColor);

    // Thủy — main wave triangle, bottom-right.
    Vector2 thuyV1 = { ir,           ib };
    Vector2 thuyV2 = { ir,           ib - capSize };
    Vector2 thuyV3 = { ir - capSize, ib };
    DrawTriangle(thuyV1, thuyV2, thuyV3, thuyColor);

    // Stacked wave-curl highlight on awake states only.
    if (awake) {
        const float curlOff  = 6.0f;
        const float curlSize = capSize * 0.55f;
        Color curlColor = Theme::withAlpha(thuyColor, 200);
        Vector2 cV1 = { ir - curlOff,            ib - curlOff };
        Vector2 cV2 = { ir - curlOff,            ib - curlOff - curlSize };
        Vector2 cV3 = { ir - curlOff - curlSize, ib - curlOff };
        DrawTriangle(cV1, cV2, cV3, curlColor);
    }

    int tw = Fonts::measure(Fonts::bold, label, kBtnTextSize);
    int tx = static_cast<int>(outer.x + (outer.width - static_cast<float>(tw)) * 0.5f);
    int ty = static_cast<int>(outer.y + (outer.height - kBtnTextSize) * 0.5f);
    Fonts::draw(Fonts::bold, label, tx, ty, kBtnTextSize, textColor);
}

void drawTitle(const char* label, int screenW, int screenH, int yTop) {
    int tw = Fonts::measure(Fonts::title, label, kTitleSize);
    int x  = (screenW - tw) / 2;
    int y  = (yTop < 0) ? screenH / 6 : yTop;
    Fonts::draw(Fonts::title, label, x, y, kTitleSize, WHITE);
}

void drawHintBar(const char* hint, int screenW, int screenH, bool withBackdrop) {
    int hw = Fonts::measure(Fonts::body, hint, kHintSize);
    int hx = (screenW - hw) / 2;
    int hy = screenH - 28;

    if (withBackdrop) {
        Rectangle hp = { static_cast<float>(hx - 12),
                         static_cast<float>(hy - 4),
                         static_cast<float>(hw + 24),
                         22.0f };
        DrawRectangleRounded(hp, 0.5f, 4,
                             Theme::withAlpha(Theme::palette.ink_sumi, 160));
    }
    Fonts::draw(Fonts::body, hint, hx, hy, kHintSize,
                Theme::withAlpha(Theme::palette.son_bone, 220));
}

void drawComicPanel(const ComicPanel& p, int xCenter, int yTop) {
    int W = (p.widthCh > 0) ? p.widthCh : 50;
    int textW = W - 6;  // 2 frame chars + 2 chars padding each side
    if (textW < 8) textW = 8;

    std::string out;
    out.reserve(static_cast<size_t>(W) * 32u * 4u);
    out += "╔" + repeatGlyph("═", W - 2) + "╗\n";

    if (p.title) {
        std::string left  = std::string("  ") + p.title;
        std::string right = p.tag ? std::string("[") + p.tag + "]  "
                                  : std::string("  ");
        int gap = (W - 2) - cpCount(left.c_str()) - cpCount(right.c_str());
        if (gap < 1) gap = 1;
        std::string content = left;
        appendSpaces(content, gap);
        content += right;
        out += paddedRow(content, W, "║") + "\n";
        out += "╠" + repeatGlyph("═", W - 2) + "╣\n";
    }

    out += paddedRow("", W, "║") + "\n";

    // Plot block — verbatim, no wrap. Lines longer than textW are truncated
    // by the caller. Each plot line gets a 2-space indent inside the frame.
    if (p.plot && p.plot[0]) {
        const char* q = p.plot;
        std::string line;
        for (;;) {
            char c = *q;
            if (c == '\n' || c == '\0') {
                out += paddedRow(std::string("  ") + line, W, "║") + "\n";
                line.clear();
                if (c == '\0') break;
                ++q;
                continue;
            }
            line += c;
            ++q;
        }
        out += "╟" + repeatGlyph("─", W - 2) + "╢\n";
        out += paddedRow("", W, "║") + "\n";
    }

    renderWrappedBody(out, wrapBody(p.body, textW), W, "║");
    out += paddedRow("", W, "║") + "\n";
    out += "╚" + repeatGlyph("═", W - 2) + "╝";

    int width = Fonts::measure(Fonts::mono, out.c_str(), kPanelTextSize, 0.0f);
    Fonts::draw(Fonts::mono, out.c_str(),
                xCenter - width / 2, yTop, kPanelTextSize,
                Theme::palette.son_bone, 0.0f);
}

void drawDialoguePanel(const DialoguePanel& p, int xCenter, int yTop) {
    int W = (p.widthCh > 0) ? p.widthCh : 40;
    int textW = W - 4;
    if (textW < 8) textW = 8;

    std::string out;
    out.reserve(static_cast<size_t>(W) * 16u * 4u);
    out += "╭" + repeatGlyph("─", W - 2) + "╮\n";
    renderWrappedBody(out, wrapBody(p.body, textW), W, "│");
    out += "╰" + repeatGlyph("─", W - 2) + "╯\n";
    out += "       \\\n";
    out += "        \\\n";
    out += avatarFor(p.speaker);

    int width = Fonts::measure(Fonts::mono, out.c_str(), kPanelTextSize, 0.0f);
    Fonts::draw(Fonts::mono, out.c_str(),
                xCenter - width / 2, yTop, kPanelTextSize,
                Theme::palette.son_bone, 0.0f);
}

}  // namespace UIC
