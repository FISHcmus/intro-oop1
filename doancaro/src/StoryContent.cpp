#include "StoryContent.h"

namespace StoryContent {

// Intro monologue — 13 paragraphs grouped into 6 thematic pages.
const char* const kIntroPages[kIntroPageCount] = {

    // Page 1 — modern persona
    "Tôi là Đại Sử Tiên — phần đông người ta biết đến ta qua một cái tên "
    "khác là Mỵ Nương.\n\n"
    "Mỗi sớm, tôi vẫn đi một chuyến xe quen, ghé một hàng cà phê cũ nơi "
    "góc phố — cái quán ấy đã mở rồi đóng, đóng rồi lại mở, ba bận trong "
    "ba mươi năm. Trông bề ngoài, tôi cũng chẳng khác chi muôn vạn kẻ "
    "đang sinh tồn trên dải đất hình chữ S này. Có điều, trong tôi giấu "
    "một điều mà người đời khó lòng tưởng tượng nổi.",

    // Page 2 — origin (father)
    "Dù ai cũng biết đến tên ta qua những câu chuyện thần thoại trong "
    "sách giáo khoa lớp một, song chẳng mấy ai thật sự biết ta là ai. "
    "Ta là đứa con gái duy nhất của vị thần mà các ngươi quen gọi Lạc "
    "Long Quân, mà ta gọi là Cha. Ta đã ở đây từ trước Âu Cơ, từ trước "
    "trăm trứng, từ trước cả khi câu chuyện về cõi này được kể lần đầu.\n\n"
    "Cha ta không chết theo cái nghĩa mà người đời hằng hiểu. Ngài là "
    "vị cổ thần khai thiên lập địa lên vùng đất này. Đến khi công việc "
    "đã thành, Ngài mới tan vào lòng đất. Xương Ngài hoá thành dãy "
    "Trường Sơn. Máu Ngài thấm xuống đất đỏ miền cao. Hơi thở Ngài "
    "ngưng tụ thành ngọn gió nồm thổi mãi trên biển Đông. Ngài không "
    "mất đi — Ngài chính là tất cả những gì các ngươi đang đứng trên đó.",

    // Page 3 — the lie
    "Suốt mấy nghìn năm, tôi tồn tại dưới nhiều nhân dạng — kẻ chép sử, "
    "người chứng kiến. Ta chẳng bao giờ can thiệp vào dòng chảy lịch sử. "
    "Ấy là điều tôi hằng tự nhủ.",

    // Page 4 — twin spirits, primal split
    "Có hai vị đại tinh linh được sinh ra trong khoảnh khắc Cha tôi tan "
    "vào đất. Sơn Tinh — hiện thân của núi, của đất khô và rễ cây bám "
    "sâu. Thủy Tinh — hiện thân của nước, của triều dâng và bão cuốn. "
    "Mỗi viên đá Sơn Tinh đặt xuống là một mảnh đất được định hình. "
    "Mỗi con sóng Thủy Tinh dấy lên là một vùng đất bị nhấn chìm.\n\n"
    "Có một điều ít kẻ hay: thuở hỗn mang trước khi trời đất phân định, "
    "Sơn và Thủy vốn là một — chưa từng phân biệt đất với nước, núi với "
    "biển. Đến khi Cha tôi hy sinh thân mình mà định hình vũ trụ, sự "
    "hoá thân của Ngài xé toạc cả hai ra thành hai thực thể riêng biệt. "
    "Từ đó, Sơn và Thủy mang trong mình vết thương của sự phân ly — "
    "tương khắc, song chẳng thể tồn tại mà thiếu nhau.",

    // Page 5 — gieo quẻ, contest, Sơn wins
    "Cả hai đến tìm ta, đòi quyền cai quản cõi này. Đây là một quyết "
    "định khó khăn. Ta lập đàn gieo quẻ bát quái, lấy kết quả thuận "
    "theo ý trời. Trời phán rằng:\n\n"
    "  \"Trong hai ngươi, kẻ mà cõi này đã chọn — Voi chín ngà, Gà chín "
    "cựa, Ngựa chín hồng mao sẽ tự tìm đến tay người ấy. Ta chỉ chờ xem.\"\n\n"
    "Sơn Tinh dễ dàng kiếm đủ cả ba. Còn Thủy Tinh, vì lòng chấp mê bất "
    "ngộ, đưa quân đến cướp lễ vật. Trận giao chiến long trời lở đất, "
    "xé toạc bầu trời. Cuối cùng, Sơn Tinh thắng. Ta trao quyền cai "
    "quản sự ấm no của cõi này cho người ấy.",

    // Page 6 — eternal war, present, vow break
    "Song Thủy Tinh chưa bao giờ ngưng sân hận. Suốt bốn nghìn năm, mỗi "
    "độ mưa bão, y lại dấy quân lên, nhấn cả cõi này chìm trong biển "
    "nước. Cuộc binh đao ấy chưa từng tận. Chỉ tạm dừng mà thôi.\n\n"
    "Và bây giờ, mùa mưa bão lại đến.\n\n"
    "Sơn Tinh. Ta không ghi chép nữa.",
};

// ---- Per-set narration ----

const SetText kSet1 = {
    "MƯA GIÔNG",
    "SET 1 · DỄ",
    "Mưa giông lại kéo đến, nhưng không giống như những lần trước. Thủy "
    "Tinh tạo ra một cơn mưa lớn kéo dài hàng tháng trời ở vùng đất Sài "
    "Gòn. Hắn chưa bao giờ làm thế trước đây — chắc hẳn là có điều gì "
    "đó đáng ngờ.",

    "Thủy Tinh đã bị đánh bại, nhưng đấy không phải là trận đánh cuối "
    "cùng. Hắn đang âm mưu phá huỷ Long Mạch — nhờ có nó mà suốt hàng "
    "nghìn năm nay đất nước này luôn được phù hộ vượt qua mọi thiên "
    "tai. Nếu hắn phá huỷ được Long Mạch, hắn sẽ làm những thứ không "
    "ai có thể tưởng tượng được.",

    "Thủy Tinh nhanh chóng chiếm lĩnh toàn bộ Việt Nam.",
};

const SetText kSet2 = {
    "CUỒNG PHONG",
    "SET 2 · VỪA",
    "Hắn đang ra sức tấn công Long Mạch ở Long An. Với một cơn lốc xoáy "
    "khổng lồ cùng hàng triệu binh lính đến từ biển sâu. Chúng ta phải "
    "chặn hắn bằng mọi giá. Sơn Tinh, hãy cưỡi Voi chín ngà đi nghênh "
    "chiến.",

    "Dù chúng ta đã đẩy lùi được đạo quân của Thủy Tinh, hắn đã thành "
    "công trong việc làm tổn thương Long Mạch, và sẽ huy động toàn bộ "
    "đạo quân cho trận đánh cuối cùng. May thay, phần Long Mạch cuối "
    "cùng hy sinh chính nó để đánh thức linh vật thứ hai: Gà chín cựa.",

    "Sơn Tinh ngã xuống trong cuộc chiến huyền thoại bảo vệ Long Mạch. "
    "Cả thiên hạ chìm trong biển nước.",
};

const SetText kSet3 = {
    "CƠN BÃO TẬN THẾ",
    "SET 3 · KHÓ",
    "Hàng triệu quái vật đến từ biển khơi đang dồn cuộc tổng lực cuối "
    "cùng để thanh tẩy vùng đất Sài Gòn. Đó chính là mục đích cuối "
    "cùng của Thủy Tinh — y muốn san phẳng mọi thứ, muốn tất cả phải "
    "chìm xuống biển sâu. Và khi mọi thứ đã được thanh tẩy, y sẽ tạo "
    "ra một thế giới mới nơi mà y là vua của thiên hạ.",

    "Từ dưới biển sâu, Thủy Tinh xuất hiện. Y mạnh đến nỗi chỉ một cú "
    "vẫy tay, cả một quận đã bị nhấn chìm trong biển nước. Cùng lúc "
    "đó, trên trời cao, Ngựa chín hồng mao giáng thế giúp ngươi đánh "
    "bại boss cuối cùng.",

    "Sơn Tinh đã ngã xuống, kéo theo hệ quả là cả thiên hạ chìm xuống "
    "biển sâu.",
};

const SetText kFinalBoss = {
    "CHÂN HÌNH THUỶ TINH",
    "BOSS · TỬ ĐẤU",
    "Thuỷ Tinh hiện nguyên hình — một con quái vật khổng lồ với chín "
    "đầu rồng mọc trên thân bạch tuộc. Y thách thức trực tiếp ngươi "
    "trong trận chiến cuối cùng. Y có quyền năng riêng: cứ bốn nước "
    "ngươi đặt xuống, y xé toạc bốn quân của ngươi khỏi bàn cờ. Đừng "
    "quên — ngươi vẫn còn linh vật trong tay.",

    "Thuỷ Tinh lại bị hạ nhục, như hàng nghìn lần trong lịch sử. Nhưng "
    "lần này, y trở nên suy yếu đến mức cơ thể y hoà tan vào làm một "
    "với biển khơi. Sẽ mất hàng chục năm để y phục hồi như ban đầu. "
    "Ít nhất vùng đất này có thể yên ổn thêm một khoảng thời gian.",

    "Người ta nói cái thiện sẽ chiến thắng cái ác. Tôi, như một đại sử "
    "gia đã sống hàng nghìn năm, không tin vào điều đó. Cái ác luôn "
    "chiến thắng cái thiện. Và công sức hàng nghìn năm xây dựng nên "
    "nền văn minh này đã tiêu vong.",
};

// ASCII art block for FinalBoss intro — drawn in ComicPanel.plot.
// 64 cols max, 24 lines. 9 dragon heads on octopus body.
const char* const kThuyTinhBossArt =
    "    ╲╱   ╲╱   ╲╱   ╲╱   ╲╱   ╲╱   ╲╱   ╲╱   ╲╱       \n"
    "    ▼    ▼    ▼    ▼    ▼    ▼    ▼    ▼    ▼        \n"
    "   ╱██╲ ╱██╲ ╱██╲ ╱██╲ ╱██╲ ╱██╲ ╱██╲ ╱██╲ ╱██╲      \n"
    "  ◉ ◉│ ◉ ◉│ ◉ ◉│ ◉ ◉│ ◉ ◉│ ◉ ◉│ ◉ ◉│ ◉ ◉│ ◉ ◉│       \n"
    "   ╲═╱  ╲═╱  ╲═╱  ╲═╱  ╲═╱  ╲═╱  ╲═╱  ╲═╱  ╲═╱       \n"
    "    │    │    │    │    │    │    │    │    │        \n"
    "     ╲    ╲    ╲    ╲   │   ╱    ╱    ╱    ╱         \n"
    "      ╲    ╲    ╲    ╲ │ ╱    ╱    ╱    ╱            \n"
    "       ╲════╲════╲════╲│╱════╱════╱════╱             \n"
    "                  ╔═════════╗                        \n"
    "                  ║ ◉  ▼  ◉ ║                        \n"
    "                  ║         ║                        \n"
    "                  ║  ╲═══╱  ║                        \n"
    "                  ║   ███   ║                        \n"
    "                  ╚═════════╝                        \n"
    "              ╱╲   ╱╲   ╱╲   ╱╲                      \n"
    "             ╱  ╲ ╱  ╲ ╱  ╲ ╱  ╲                     \n"
    "            ╱    ╳    ╳    ╳    ╲                    \n"
    "           ~~~~~~~~~~~~~~~~~~~~~~~                   \n"
    "         BIỂN SÂU TRỖI DẬY";

// ---- Linh vật unlock lore ----

const char* const kVoiUnlockLine =
    "Voi chín ngà vốn là một trong ba lễ vật Hùng Vương đặt ra thuở "
    "trước. Khi đã về tay Sơn Tinh, nó lui ẩn vào lòng đất, mất tích "
    "hàng nghìn năm — người đời sau chỉ còn nghe tên trong sử sách.\n\n"
    "Đến năm Hai Bà Trưng thua trận tại sông Hát, máu Trưng Trắc thấm "
    "vào thân con bạch tượng mà Bà đã cưỡi. Xác con voi trở thành nơi "
    "để Thần thú Voi Chín Ngà tái sinh. Sử sách ghi lại rằng voi thần "
    "đã bảo vệ bộ lạc của Hai Bà khỏi nạn thảm sát của giặc phương "
    "Bắc.\n\n"
    "Nó cho ngươi sức mạnh của ký ức — hoàn tác năm nước cờ đã đặt. "
    "Khi thắng thua đã định, voi không can dự.";

const char* const kGaUnlockLine =
    "Gà chín cựa cũng là một lễ vật xưa của Hùng Vương. Sau khi về tay "
    "Sơn Tinh, nó biến mất vào sương mây núi Bắc, mất tích hàng trăm "
    "năm.\n\n"
    "Đến đời An Dương Vương xây thành Cổ Loa, đêm đêm tường thành cứ "
    "dựng lên rồi sụp xuống. Trên núi Thất Diệu, một con yêu kê đầu "
    "trắng được cho là đã sống nghìn năm, mỗi đêm gáy ba tiếng, đất "
    "trời rung chuyển — chính là Gà chín cựa đã hoá yêu trong cô tịch "
    "của núi sâu. Nhờ Thần Kim Quy chỉ đường, vua đến chân núi mà cảm "
    "hoá, không giết. Gà cúi đầu, hứa rằng đến ngày giặc trời giặc đất "
    "kéo đến, sẽ trở về một lần.\n\n"
    "Lần ấy là bây giờ. Tiếng gáy của nó làm đối phương loạn trí, một "
    "lượt đi không theo lẽ thường. Ngươi gọi được ba bận.";

const char* const kNguaUnlockLine =
    "Ngựa chín hồng mao là lễ vật cuối cùng trong ba thứ Hùng Vương "
    "đặt ra thuở trước. Sau khi Sơn Tinh ngự về, nó hoá vào gió nồm, "
    "lưu lạc bốn phương tám hướng.\n\n"
    "Đến đời giặc Ân kéo xuống nước Nam, một đứa bé ba tuổi vùng Phù "
    "Đổng vươn vai thành tráng sĩ. Vua trao ngựa sắt, roi sắt — song "
    "ít ai hay rằng dưới lớp sắt ấy là chính Ngựa chín hồng mao, mượn "
    "thân kim khí để xuôi gió đánh giặc. Khi giặc đã tan, sắt rơi rụng "
    "theo bước chân, ngựa cùng tráng sĩ phi thẳng về trời. Đến nay, "
    "dấu chân nó vẫn còn in trên đỉnh núi Sóc.\n\n"
    "Nay ngươi gọi đến nó một lần. Nếu ngươi ngã xuống, ngựa kéo ngươi "
    "về năm lượt trước, cho ngươi đứng dậy mà đánh tiếp. Một bận, "
    "không hai.";

const char* const kEpilogueLine =
    "Ngươi đã giữ vững giang sơn. Ta ghi vào sử sách.\n\n"
    "Mùa mưa bão này tạm dừng. Thủy Tinh sẽ trở lại — y luôn trở lại. "
    "Nhưng đến lúc ấy, ngươi đã có ba linh vật bên mình, và ta vẫn còn "
    "ở đây, chép tiếp.";

}  // namespace StoryContent
