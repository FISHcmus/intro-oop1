#!/usr/bin/env python3

from __future__ import annotations

import re
import shutil
import subprocess
import tempfile
from pathlib import Path

import officehelper
import uno
from com.sun.star.beans import PropertyValue


ROOT = Path("/home/larvartar/nhannht-projects/hcmus/semester2/intro-oop1")
DOCX_TEMPLATE = ROOT / "doancaro" / "Mau bai Tieu luan mon hoc.docx"
REPORT_MD = ROOT / "doancaro" / "report" / "our_work" / "bao-cao-do-an-caro.md"
OUTPUT_DOCX = ROOT / "doancaro" / "Bao cao do an Caro.docx"
TITLE = "XÂY DỰNG TRÒ CHƠI CARO (GOMOKU) 15×15\nVỚI ĐỒ HỌA 3D VÀ TRÍ TUỆ NHÂN TẠO"
COURSE = "MÔN NHẬP MÔN LẬP TRÌNH HƯỚNG ĐỐI TƯỢNG"
TEACHER = "GIẢNG VIÊN HƯỚNG DẪN: THS. TRƯƠNG TOÀN THỊNH"
CLASS_NAME = "Công nghệ giáo dục - Khoa Khoa học liên ngành"
CITY_MONTH = "Tp. Hồ Chí Minh, tháng 05 năm 2026"
REVIEW_DATE = "Tp. HCM, ngày …… tháng …… năm 2026"
STUDENTS = [
    ("Nguyễn Hữu Thiện Nhân", "25310023"),
    ("Bùi Thị Minh Hằng", "25310057"),
    ("Phạm Ngọc Trâm", "25310043"),
]


def prop(name: str, value):
    p = PropertyValue()
    p.Name = name
    p.Value = value
    return p


def extract_body_markdown() -> str:
    text = REPORT_MD.read_text(encoding="utf-8")
    marker = "## Lời cảm ơn"
    if marker not in text:
        raise RuntimeError(f"Missing marker {marker!r} in report markdown")
    body = text[text.index(marker):]
    body = body.replace("```mermaid", "```text")
    return body


def generate_body_docx(workdir: Path) -> Path:
    body_md = workdir / "body.md"
    body_docx = workdir / "body.docx"
    body_md.write_text(extract_body_markdown(), encoding="utf-8")
    subprocess.run(
        [
            "pandoc",
            str(body_md),
            "--from=gfm",
            "--to=docx",
            "--standalone",
            "--resource-path",
            str(REPORT_MD.parent),
            "--output",
            str(body_docx),
        ],
        check=True,
        cwd=REPORT_MD.parent,
    )
    return body_docx


def build_cover_text() -> str:
    lines = [
        "ĐẠI HỌC QUỐC GIA TP. HỒ CHÍ MINH",
        "TRƯỜNG ĐẠI HỌC KHOA HỌC TỰ NHIÊN",
        "",
        "",
        "BÁO CÁO ĐỒ ÁN CUỐI KỲ",
        COURSE,
        "",
        TITLE,
        "",
        TEACHER,
        "",
    ]
    for name, student_id in STUDENTS:
        lines.append(f"SVTH: {name}    MSSV: {student_id}")
    lines.extend(
        [
            f"LỚP: {CLASS_NAME}",
            "",
            "",
            CITY_MONTH,
            "",
        ]
    )
    return "\n".join(lines)


def bootstrap_desktop():
    ctx = officehelper.bootstrap()
    return ctx.ServiceManager.createInstanceWithContext("com.sun.star.frame.Desktop", ctx)


def replace_cover_shape(doc) -> None:
    draw_page = doc.getDrawPage()
    for index in range(draw_page.getCount()):
        shape = draw_page.getByIndex(index)
        if not hasattr(shape, "getString"):
            continue
        text = shape.getString()
        if "MÔN HỌC LỊCH SỬ ĐẢNG CỘNG SẢN VIỆT NAM" not in text:
            continue
        shape.setString(build_cover_text())
        return
    raise RuntimeError("Could not find the cover text shape in template")


def replace_review_page(doc) -> None:
    enum = doc.Text.createEnumeration()
    while enum.hasMoreElements():
        para = enum.nextElement()
        current = para.getString().strip()
        if current == "<TÊN CHỦ ĐỀ>":
            para.setString("ĐỀ TÀI: " + TITLE.replace("\n", " "))
        elif current == "BÀI LÀM":
            para.setString("NHẬN XÉT VÀ ĐÁNH GIÁ")
        elif current == "Giảng viên đánh giá":
            para.setString("Giảng viên đánh giá")
        elif current == "Tp. HCM, ngày …… tháng …… năm 2026":
            para.setString(REVIEW_DATE)


def append_body_document(doc, body_docx: Path) -> None:
    cursor = doc.Text.createTextCursorByRange(doc.Text.getEnd())
    page_break = uno.getConstantByName("com.sun.star.text.ControlCharacter.PAGE_BREAK")
    doc.Text.insertControlCharacter(cursor, page_break, False)
    cursor.insertDocumentFromURL(body_docx.as_uri(), ())


def set_document_metadata(doc) -> None:
    props = doc.getDocumentProperties()
    props.Title = "Báo cáo đồ án Caro"
    props.Subject = "Nhập môn Lập trình Hướng đối tượng"
    props.Description = "Báo cáo đồ án cuối kỳ Caro (Gomoku) 15x15"


def build_output_docx() -> Path:
    desktop = bootstrap_desktop()
    with tempfile.TemporaryDirectory(prefix="caro-report-docx-") as tmpdir:
        workdir = Path(tmpdir)
        body_docx = generate_body_docx(workdir)
        source_uri = DOCX_TEMPLATE.as_uri()
        load_props = (prop("Hidden", True), prop("ReadOnly", True))
        doc = desktop.loadComponentFromURL(source_uri, "_blank", 0, load_props)
        try:
            if OUTPUT_DOCX.exists():
                OUTPUT_DOCX.unlink()
            doc.storeAsURL(
                OUTPUT_DOCX.as_uri(),
                (prop("FilterName", "Office Open XML Text"),),
            )
            replace_cover_shape(doc)
            replace_review_page(doc)
            append_body_document(doc, body_docx)
            set_document_metadata(doc)
            doc.store()
        finally:
            doc.close(True)
    return OUTPUT_DOCX


if __name__ == "__main__":
    output = build_output_docx()
    print(output)
