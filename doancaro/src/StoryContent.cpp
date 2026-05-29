#include "StoryContent.h"

namespace StoryContent {

// Intro monologue — 8 pages, paced for the ComicPanel layout.
const char* const kIntroPages[kIntroPageCount] = {

    // Page 1 — the mask
    "Tôi là Đại Sử Tiên. Người đời, nếu còn nhớ, thì nhớ bằng một cái "
    "tên vừa đẹp vừa nhiều tai vạ: Mỵ Nương.\n\n"
    "Bây giờ tôi sống như một người đàn bà đã qua tuổi trẻ ở Sài Gòn. "
    "Sáng ngồi trong một quán cà phê nhỏ, nghe tiếng muỗng chạm thành "
    "ly, nghe mùi hạt rang quyện với mùi nhựa đường sau cơn mưa đêm. "
    "Ngoài cửa kính, người ta chen nhau đi làm, ai cũng vội. Cái vội "
    "của người sống từng ngày thường làm tôi thấy thương.",

    // Page 2 — ordinary life above an old fault line
    "Họ đâu cần biết dưới lớp nhựa, lớp cống, lớp móng nhà kia còn "
    "chôn những chuyện cũ gì. Có biết cũng chẳng ích gì. Người ta còn "
    "bận tiền trọ, bận bữa cơm, bận một đứa con sốt giữa mùa mưa.\n\n"
    "Chỉ có tôi, hễ đến những ngày nước chưa dâng mà miệng cống đã "
    "run, lại nghe trong lòng đất một tiếng thở dài rất cũ. Những lúc "
    "ấy, tôi nhớ ra mình đã sống lâu hơn nhiều triều đại.",

    // Page 3 — father as geography
    "Ta là con gái của người mà đời sau gọi là Lạc Long Quân. Với các "
    "ngươi, đó là chuyện thần thoại. Với ta, ấy là chuyện nhà.\n\n"
    "Ngài không chết như người ta chết. Khi việc mở cõi đã xong, Ngài "
    "gửi thân vào đất: lưng thành núi, máu thành phù sa, hơi thở thành "
    "gió nồm. Từ bấy đến nay, thiên hạ đi trên Người mà không biết.",

    // Page 4 — chronicler's lie
    "Bốn nghìn năm, tôi đổi tên như người ta thay áo cũ. Lúc làm kẻ "
    "chép sử, lúc làm người đứng xem, lúc chỉ là một cái bóng đi ngang "
    "một triều đại rồi mất trước khi nó đổ.\n\n"
    "Tôi tự bảo mình vô can. Người ta khổ, người ta chết, người ta "
    "trôi, còn tôi chỉ viết. Cái \"chỉ viết\" ấy nghe hiền lành, mà "
    "thực ra rất độc. Lời nói dối nào lập đi lập lại đủ lâu cũng thành "
    "chỗ trú cho lương tâm.",

    // Page 5 — primal split
    "Ngày Cha rã mình vào đất, từ một vết thương sinh ra hai thứ sức "
    "mạnh. Một đằng là đất, là rễ, là cái bền bỉ lầm lì của núi. Một "
    "đằng là nước, là triều, là cái hăng hái tàn nhẫn của biển. Người "
    "đời gọi họ là Sơn Tinh và Thủy Tinh.\n\n"
    "Nói cho đúng, họ vốn không phải hai. Họ bị tách ra. Mà cái gì bị "
    "xé làm đôi thì chẳng đau. Có điều, có nỗi đau chảy thành lũ.",

    // Page 6 — judgment
    "Hai người tìm đến ta thuở ấy. Thiên hạ kể là đến cầu hôn. Kể như "
    "thế cũng phải, nhưng chưa đủ. Họ còn đến để hỏi xem cõi này nên "
    "thuộc về ai: kẻ biết đắp đất cho người ta ở, hay kẻ muốn kéo hết "
    "thảy về chỗ nước sâu.\n\n"
    "Ta bày quẻ, bày sính lễ, bày luôn cả sự thiên vị mà khi ấy chính "
    "ta cũng không dám gọi tên. Voi chín ngà, gà chín cựa, ngựa chín "
    "hồng mao. Ta cứ ngỡ mình chỉ thay trời nói hộ một câu.",

    // Page 7 — first war, hidden guilt
    "Sơn Tinh đến trước. Thủy Tinh đến sau. Chỉ chậm một khắc thôi. Ở "
    "đời, lắm khi một khắc đã đủ cho một người nên thân, mà cũng đủ "
    "cho một kẻ hóa dữ.\n\n"
    "Từ hôm ấy, năm nào nước cũng kéo về. Nước dâng đến đâu, núi lại "
    "đội lên đến đấy. Người đời bảo thế là thắng. Ta thì biết: có "
    "những cái thắng giữ được đất mà không chôn nổi một mối hờn. Mối "
    "hờn ấy sống dai như cỏ dại ven sông.",

    // Page 8 — present-day threat, vow break
    "Năm nay khác. Y không chỉ muốn cướp lại một người đàn bà đã thành "
    "chuyện cũ. Y đang lần theo long mạch dưới lòng đất, như kẻ tìm "
    "đúng chỗ đau mà xoáy ngón tay vào.\n\n"
    "Nếu vết thương đầu tiên bị mở lại, không chỉ một mùa lũ đâu. Cả "
    "xứ sở này sẽ quên mất cách đứng. Sơn Tinh, lần này ta không viết "
    "nữa. Ta theo ngươi.",
};

// ---- Per-set narration ----

const SetText kSet1 = {
    "MƯA TRÁI MÙA",
    "SET 1 · DỄ",
    "Mưa chưa tới mà nước đã ngoi lên ở miệng cống. Người bán vé số ôm "
    "xấp vé chạy trú dưới mái hiên. Người đi làm kéo ống quần lên, lội "
    "qua một vũng nước mới một giờ trước còn là mặt đường. Thủy Tinh "
    "chưa đánh lớn. Y mới chỉ thử chân đất của thành phố, xem chỗ nào "
    "mềm nhất.",

    "Ngươi chặn được đợt đầu. Nhưng trong lúc người ta còn mừng vì nước "
    "rút, y đã nghe ra nhịp của long mạch miền Nam. Lần sau, y không "
    "đánh vào phố xá nữa. Y sẽ đánh vào cái chỗ khiến đất còn là đất. "
    "Mà một khi đất đã quên mình, người sống trên đó cũng khó mà đứng "
    "thẳng.",

    "Nước vào nhà nhanh như một tin dữ. Đèn tắt, bếp nguội, trẻ con "
    "khóc ngằn ngặt. Thành phố tưởng mình mạnh lắm, hóa ra cũng chỉ là "
    "một chỗ ở tạm bợ trước mặt nước.",
};

const SetText kSet2 = {
    "VẾT NỨT LONG MẠCH",
    "SET 2 · VỪA",
    "Long An là một mắt xương của Cha. Chỗ ấy đất mỏng, ký ức dồn dày. "
    "Thủy Tinh khoan nước vào đó ngày đêm, như kẻ cố bới một nấm mồ "
    "cho bật nắp. Sơn Tinh, cưỡi Voi chín ngà mà đi. Trận này không "
    "phải giữ một thửa ruộng hay một con đường. Trận này là giữ cho xứ "
    "sở còn nhớ nó vốn đứng trên đất, chứ không phải trên một cơn lũ "
    "chậm.",

    "Ngươi giữ được long mạch, nhưng không giữ được nó nguyên lành. Vết "
    "nứt đã há miệng rồi. Nước chưa vào ngay, nhưng cái lạnh của nó đã "
    "thấm. Cũng từ chỗ rạn ấy, Gà chín cựa cất tiếng. Bình minh còn "
    "khuya, song ít nhất đêm nay đã biết sợ.",

    "Long mạch vỡ. Từ vết vỡ ấy, nước kéo đi ràn rạt như đàn chó đã "
    "quen mùi máu. Bờ bãi, xóm làng, ruộng vườn rồi sẽ thành một thứ "
    "bùn không tên.",
};

const SetText kSet3 = {
    "BIỂN DỰNG THÀNH",
    "SET 3 · KHÓ",
    "Bây giờ y bỏ hẳn cái lối quấy phá từng nơi. Y dựng cả mặt biển "
    "lên, định ép Sài Gòn làm chứng cho một điều rất cũ mà cũng rất ác: "
    "thứ gì sinh từ nước thì rốt cuộc phải trả về nước. Nếu thành phố "
    "này cúi đầu, nhiều nơi khác sẽ quỳ theo. Người ta sợ không phải "
    "vì nước sâu, mà vì thấy mình bỗng bé nhỏ và vô ích.",

    "Ngươi bẻ gãy được bức tường nước. Thế là đủ để Thủy Tinh phải tự "
    "bước ra, không nấp sau mưa gió nữa. Ngựa chín hồng mao đã hạ vó "
    "trên tầng mây. Trận sau không còn là chống lũ. Là thanh toán một "
    "món nợ đã kéo qua mấy nghìn năm.",

    "Biển dựng thành. Thành hóa mồ. Từ đêm ấy, bản đồ chỉ còn là thứ "
    "giấy chép lại một chỗ ở đã mất.",
};

const SetText kFinalBoss = {
    "CHÂN HÌNH CỬU THỦ",
    "BOSS · TỬ ĐẤU",
    "Y hiện nguyên hình. Không còn mưa gió làm màn, không còn quân lính "
    "làm đệm. Chỉ còn một khối giận dữ đội nước mà đứng, chín cái đầu "
    "thay nhau ngẩng lên như chín mùa lũ chưa chịu chết. Y không cần "
    "thắng một ván cờ. Y cần xé bỏ cái phán quyết năm xưa, kéo đất "
    "liền trở lại chỗ bùn mặn đầu tiên.\n\n"
    "Cứ bốn nước ngươi đi, y lại giật bốn quân xuống vực. Y vẫn thế: "
    "đã không được thì phá cho hả. Nhưng ngươi nhớ lấy, linh vật không "
    "đến để làm trò lạ. Chúng đến vì đất này chưa muốn chết chìm.",

    "Thủy Tinh gào lên. Tiếng gào nghe như cả mặt biển bị cào rách. Rồi "
    "thân y rã dần trong nước mặn, không hẳn là bị giết, chỉ là không "
    "gượng nổi mãi một mối hờn quá nặng. Y sẽ còn trở lại. Những giống "
    "hận như y khó mà tuyệt được. Nhưng đêm nay, y phải lui.",

    "Phán quyết cũ đứt rồi. Đất trả cho nước. Tên họ, bờ bãi, mái nhà, "
    "tiếng ru con, mùa gặt, trang sử, hết thảy bị dìm lùi về cái buổi "
    "con người còn lóp ngóp học đứng trên bùn.",
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
    "Voi chín ngà là một trong ba lễ vật vua Hùng đòi thuở kén rể. Khi "
    "về với Sơn Tinh, nó lặn vào lòng đất như một khúc núi biết đi, "
    "mất hút đến nỗi người sau chỉ còn biết nhắc tên trong sách.\n\n"
    "Đến ngày Hai Bà Trưng cùng đường ở sông Hát, máu Trưng Trắc thấm "
    "xuống xác bạch tượng. Từ chỗ máu ấy, thần thú thức dậy. Người xưa "
    "nói voi thần đã che cho tàn quân khỏi một cuộc giết sạch.\n\n"
    "Nó cho ngươi lùi lại năm nước đã đặt. Nhưng khi ván cờ đã ngã "
    "ngũ, voi đứng ngoài. Thần vật cũng có cái lề của nó.";

const char* const kGaUnlockLine =
    "Gà chín cựa cũng là lễ vật cũ của vua Hùng. Về tay Sơn Tinh xong, "
    "nó bỏ vào núi Bắc, lâu dần thành chuyện nửa thực nửa ngoa.\n\n"
    "Đến đời An Dương Vương đắp Cổ Loa, đêm nào thành xây lên cũng sụp "
    "xuống. Trên núi Thất Diệu có con yêu kê đầu trắng sống đã nghìn "
    "năm, mỗi lần gáy là đất trời chuyển động. Người ta bảo ấy chính là "
    "Gà chín cựa, ở lâu trong núi mà sinh cô độc đến hóa dữ. Nhờ Thần "
    "Kim Quy chỉ lối, vua không giết, chỉ cảm hóa. Gà cúi đầu, hẹn khi "
    "nào giặc trời giặc đất kéo đến sẽ trở ra một lần.\n\n"
    "Lần ấy là lúc này. Tiếng gáy của nó làm đối phương rối trí, một "
    "lượt đi bỗng chệch khỏi lẽ thường. Ngươi gọi được ba bận.";

const char* const kNguaUnlockLine =
    "Ngựa chín hồng mao là món lễ vật cuối cùng. Về với Sơn Tinh rồi, "
    "nó tan vào gió nồm, hết ải này sang bến khác, chẳng chịu ở yên "
    "đâu.\n\n"
    "Đến đời giặc Ân kéo xuống, cậu bé Phù Đổng vươn vai thành tráng "
    "sĩ. Vua trao ngựa sắt, roi sắt; nhưng dưới lớp sắt lạnh ấy chính "
    "là Ngựa chín hồng mao mượn thân kim khí mà xuống trận. Giặc tan, "
    "ngựa cùng người phi thẳng về trời. Đến nay, dấu chân còn nằm trên "
    "núi Sóc.\n\n"
    "Nó chỉ đến với ngươi một lần. Nếu ngươi ngã, nó kéo ngươi lùi năm "
    "lượt để đứng dậy đánh tiếp. Chỉ một lần thôi. Việc sống lại, nếu "
    "dễ quá, thì hóa rẻ.";

const char* const kEpilogueLine =
    "Tôi từng tưởng chép sử là đứng ngoài lịch sử. Nghĩ thế cho nhàn "
    "thân, mà cũng cho nhẹ tội. Nhưng không viết cũng là một cách "
    "viết. Im lặng cũng là một cách theo phe.\n\n"
    "Đêm nay, tôi theo phe đất còn người ở. Theo phe những mái nhà vừa "
    "thoát nước, những bếp lửa chưa tắt, những đứa trẻ còn ngủ được "
    "một giấc yên. Mùa mưa sau nếu y lại về, tôi không trốn sau ngòi "
    "bút nữa.\n\n"
    "Ngươi giữ giang sơn. Còn tôi, từ trang sử này trở đi, xin giữ cùng "
    "ngươi phần lời thề mà đất đã chịu thay người giữ bấy lâu.";

}  // namespace StoryContent
