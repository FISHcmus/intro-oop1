#include "UIComponents.h"
#include "Fonts.h"
#include "Theme.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr float kFrameInset = 4.0f;
constexpr float kCapRest    = 14.0f;
constexpr float kCapFocus   = 18.0f;
constexpr float kPressedDx  = 2.0f;
constexpr float kBtnTextSize = 22.0f;
constexpr float kTitleSize   = 50.0f;
constexpr float kHintSize    = 14.0f;
constexpr float kPanelTextSize = 18.0f;
constexpr float kPanelLineStep = 22.0f;

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

std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    size_t i = 0;
    while (i <= text.size()) {
        size_t nl = text.find('\n', i);
        lines.push_back((nl == std::string::npos)
            ? text.substr(i)
            : text.substr(i, nl - i));
        if (nl == std::string::npos) break;
        i = nl + 1;
    }
    return lines;
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

void drawComicPanel(const ComicPanel& p, int xCenter, int yTop,
                    float maxHeight, float bodyScroll, float* outMaxScroll) {
    int W = (p.widthCh > 0) ? p.widthCh : 50;
    const bool cappedHeight = maxHeight > 0.0f;

    int textW = W - 6;  // 2 frame chars + 2 chars padding each side
    if (textW < 8) textW = 8;

    std::vector<std::string> plotLines;
    if (p.plot && p.plot[0]) {
        plotLines = splitLines(std::string(p.plot));
    }

    auto buildWrappedBodyLines = [&](int wrapWidth) {
        return splitLines(wrapBody(p.body, wrapWidth));
    };

    std::vector<std::string> bodyLines = buildWrappedBodyLines(textW);

    const int fixedRowsTop = 1 + (p.title ? 2 : 0) + 1
                           + (plotLines.empty() ? 0
                                                : static_cast<int>(plotLines.size()) + 2);
    const int fixedRowsBottom = 2;
    const float fixedHeight = (fixedRowsTop + fixedRowsBottom) * kPanelLineStep;

    float bodyViewportHeight = static_cast<float>(bodyLines.size()) * kPanelLineStep;
    if (cappedHeight) {
        float availableBody = maxHeight - fixedHeight;
        if (availableBody < kPanelLineStep) availableBody = kPanelLineStep;
        int visibleRows = std::max(1, static_cast<int>(availableBody / kPanelLineStep));
        bodyViewportHeight = visibleRows * kPanelLineStep;
    }

    float maxScroll = std::max(0.0f,
        static_cast<float>(bodyLines.size()) * kPanelLineStep - bodyViewportHeight);

    // Reserve a small gutter when the body scrolls so the scrollbar does not
    // sit on top of text.
    if (cappedHeight && maxScroll > 0.0f) {
        int gutterTextW = std::max(8, textW - 2);
        bodyLines = buildWrappedBodyLines(gutterTextW);
        bodyViewportHeight = std::max(kPanelLineStep, bodyViewportHeight);
        maxScroll = std::max(0.0f,
            static_cast<float>(bodyLines.size()) * kPanelLineStep - bodyViewportHeight);
        textW = gutterTextW;
    }

    if (outMaxScroll) *outMaxScroll = maxScroll;
    bodyScroll = std::max(0.0f, std::min(bodyScroll, maxScroll));

    const std::string topBorder = "╔" + repeatGlyph("═", W - 2) + "╗";
    const std::string midBorder = "╠" + repeatGlyph("═", W - 2) + "╣";
    const std::string plotBorder = "╟" + repeatGlyph("─", W - 2) + "╢";
    const std::string bottomBorder = "╚" + repeatGlyph("═", W - 2) + "╝";

    int panelWidth = Fonts::measure(Fonts::mono, topBorder.c_str(), kPanelTextSize, 0.0f);
    int panelX = xCenter - panelWidth / 2;
    float y = static_cast<float>(yTop);

    auto drawLine = [&](const std::string& line, float yPos) {
        Fonts::draw(Fonts::mono, line.c_str(), panelX, static_cast<int>(yPos),
                    kPanelTextSize, Theme::palette.son_bone, 0.0f);
    };

    drawLine(topBorder, y);
    y += kPanelLineStep;

    if (p.title) {
        std::string left  = std::string("  ") + p.title;
        std::string right = p.tag ? std::string("[") + p.tag + "]  "
                                  : std::string("  ");
        int gap = (W - 2) - cpCount(left.c_str()) - cpCount(right.c_str());
        if (gap < 1) gap = 1;
        std::string content = left;
        appendSpaces(content, gap);
        content += right;
        drawLine(paddedRow(content, W, "║"), y);
        y += kPanelLineStep;
        drawLine(midBorder, y);
        y += kPanelLineStep;
    }

    drawLine(paddedRow("", W, "║"), y);
    y += kPanelLineStep;

    if (!plotLines.empty()) {
        for (const std::string& line : plotLines) {
            drawLine(paddedRow(std::string("  ") + line, W, "║"), y);
            y += kPanelLineStep;
        }
        drawLine(plotBorder, y);
        y += kPanelLineStep;
        drawLine(paddedRow("", W, "║"), y);
        y += kPanelLineStep;
    }

    const float bodyTopY = y;
    if (bodyViewportHeight > 0.0f) {
        BeginScissorMode(panelX, static_cast<int>(bodyTopY), panelWidth,
                         static_cast<int>(bodyViewportHeight));
        float bodyY = bodyTopY - bodyScroll;
        for (const std::string& line : bodyLines) {
            drawLine(paddedRow(std::string("  ") + line, W, "║"), bodyY);
            bodyY += kPanelLineStep;
        }
        EndScissorMode();
        y += bodyViewportHeight;
    }

    if (maxScroll > 0.0f && bodyViewportHeight > 0.0f) {
        Rectangle track = {
            static_cast<float>(panelX + panelWidth - 12),
            bodyTopY + 3.0f,
            5.0f,
            std::max(8.0f, bodyViewportHeight - 6.0f)
        };
        float thumbH = std::max(18.0f, track.height * (bodyViewportHeight
                        / (bodyViewportHeight + maxScroll)));
        float thumbY = track.y;
        if (maxScroll > 0.0f && track.height > thumbH) {
            thumbY += (track.height - thumbH) * (bodyScroll / maxScroll);
        }
        DrawRectangleRounded(track, 0.8f, 4,
                             Theme::withAlpha(Theme::palette.slate_fog, 110));
        DrawRectangleRounded({track.x, thumbY, track.width, thumbH}, 0.8f, 4,
                             Theme::withAlpha(Theme::palette.gold_foil, 220));
    }

    drawLine(paddedRow("", W, "║"), y);
    y += kPanelLineStep;
    drawLine(bottomBorder, y);
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
