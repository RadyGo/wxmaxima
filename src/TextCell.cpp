﻿// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2018 Gunter Königsmann <wxMaxima@physikbuch.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  SPDX-License-Identifier: GPL-2.0+

/*! \file
  This file defines the class TextCell

  TextCell is the Cell type that is used in order to display text that is
  contained in maxima's output.
 */

#include "TextCell.h"
#include "wx/config.h"

TextCell::TextCell(Cell *parent, Configuration **config, CellPointers *cellPointers,
                   wxString text, TextStyle style) : Cell(parent, config, cellPointers)
{
  m_alt = false;
  m_altJs = false;
  switch(m_textStyle = style)
  {
  case TS_DEFAULT: m_type = MC_TYPE_DEFAULT; break;
  case TS_VARIABLE: m_type = MC_TYPE_DEFAULT; break;
  case TS_NUMBER: m_type = MC_TYPE_DEFAULT; break;
  case TS_FUNCTION: m_type = MC_TYPE_DEFAULT; break;
  case TS_SPECIAL_CONSTANT: m_type = MC_TYPE_DEFAULT; break;
  case TS_GREEK_CONSTANT: m_type = MC_TYPE_DEFAULT; break;
  case TS_STRING: m_type = MC_TYPE_DEFAULT; break;
  case TS_INPUT: m_type = MC_TYPE_INPUT; break;
  case TS_MAIN_PROMPT: m_type = MC_TYPE_MAIN_PROMPT; break;
  case TS_OTHER_PROMPT: m_type = MC_TYPE_PROMPT; break;
  case TS_LABEL: m_type = MC_TYPE_LABEL; break;
  case TS_USERLABEL: m_type = MC_TYPE_LABEL; break;
  case TS_HIGHLIGHT: m_type = MC_TYPE_DEFAULT; break;
  case TS_WARNING: m_type = MC_TYPE_WARNING; break;
  case TS_ERROR: m_type = MC_TYPE_ERROR; break;
  case TS_TEXT: m_type = MC_TYPE_TEXT; break;
  case TS_HEADING6: m_type = MC_TYPE_HEADING6; break;
  case TS_HEADING5: m_type = MC_TYPE_HEADING5; break;
  case TS_SUBSUBSECTION: m_type = MC_TYPE_SUBSUBSECTION; break;
  case TS_SUBSECTION: m_type = MC_TYPE_SUBSECTION; break;
  case TS_SECTION: m_type = MC_TYPE_SECTION; break;
  case TS_TITLE: m_type = MC_TYPE_TITLE; break;
  default:
    wxLogMessage(wxString::Format(_("Unexpected text style %i for TextCell"),style));
    m_type = MC_TYPE_DEFAULT;
  }
  m_displayedDigits_old = -1;
  m_height = -1;
  m_realCenter = m_center = -1;
  m_lastCalculationFontSize = -1;
  m_fontSize = 10;
  m_fontSizeLabel = 10;
  m_lastZoomFactor = -1;
  TextCell::SetValue(text);
  m_highlight = false;
  m_dontEscapeOpeningParenthesis = false;
  m_initialToolTip = (*m_configuration)->GetDefaultCellToolTip();
  m_fontsize_old = -1;
}

TextCell::~TextCell()
{
  MarkAsDeleted();
}

std::list<std::shared_ptr<Cell>> TextCell::GetInnerCells()
{
  std::list<std::shared_ptr<Cell>> innerCells;
  return innerCells;
}

void TextCell::SetStyle(TextStyle style)
{
  m_widths.clear();
  Cell::SetStyle(style);
  if ((m_text == wxT("gamma")) && (m_textStyle == TS_FUNCTION))
    m_displayedText = wxT("\u0393");
  if ((m_text == wxT("psi")) && (m_textStyle == TS_FUNCTION))
    m_displayedText = wxT("\u03A8");
  if((style == TS_LABEL) || (style == TS_USERLABEL)||
     (style == TS_MAIN_PROMPT) || (style == TS_OTHER_PROMPT))
    HardLineBreak();
  ResetSize();
}

void TextCell::SetType(CellType type)
{
  m_widths.clear();
  ResetSize();
  ResetData();
  Cell::SetType(type);
}

void TextCell::SetValue(const wxString &text)
{
  m_widths.clear();
  SetToolTip(m_initialToolTip);
  m_displayedDigits_old = (*m_configuration)->GetDisplayedDigits();
  m_text = text;
  ResetSize();
  m_text.Replace(wxT("\xDCB6"), wxT("\u00A0")); // A non-breakable space
  m_text.Replace(wxT("\n"), wxEmptyString);
  m_text.Replace(wxT("-->"), wxT("\u2794"));
  m_text.Replace(wxT(" -->"), wxT("\u2794"));
  m_text.Replace(wxT(" \u2212\u2192 "), wxT("\u2794"));
  m_text.Replace(wxT("->"), wxT("\u2192"));
  m_text.Replace(wxT("\u2212>"), wxT("\u2192"));

  m_displayedText = m_text;
  if (m_textStyle == TS_FUNCTION)
  {
    if (m_text == wxT("ilt"))
      SetToolTip(_("The inverse laplace transform."));
    
    if (m_text == wxT("gamma"))
      m_displayedText = wxT("\u0393");
    if (m_text == wxT("psi"))
      m_displayedText = wxT("\u03A8");
  }      

  if (m_textStyle == TS_VARIABLE)
  {
    if (m_text == wxT("pnz"))
      SetToolTip( _("Either positive, negative or zero.\n"
                    "Normally the result of sign() if the sign cannot be determined."
                    ));

    if (m_text == wxT("pz"))
      SetToolTip(_("Either positive or zero.\n"
                    "A possible result of sign()."
                   ));
  
    if (m_text == wxT("nz"))
      SetToolTip(_("Either negative or zero.\n"
                   "A possible result of sign()."
                   ));

    if (m_text == wxT("und"))
      SetToolTip( _("The result was undefined."));

    if (m_text == wxT("ind"))
      SetToolTip( _("The result was indefinite."));

    if (m_text == wxT("zeroa"))
      SetToolTip( _("Infinitesimal above zero."));

    if (m_text == wxT("zerob"))
      SetToolTip( _("Infinitesimal below zero."));

    if (m_text == wxT("inf"))
      SetToolTip( wxT("+∞."));

    if (m_text == wxT("infinity"))
      SetToolTip( _("Complex infinity."));
        
    if (m_text == wxT("inf"))
      SetToolTip( wxT("-∞."));

    if(m_text.StartsWith("%r"))
    {
      wxString number;

      number = m_text.Right(m_text.Length()-2);

      bool isrnum = (number != wxEmptyString);
     
      for (wxString::const_iterator it = number.begin(); it != number.end(); ++it)
        if(!wxIsdigit(*it))
        {
          isrnum = false;
          break;
        }

      if(isrnum)
        SetToolTip( _("A variable that can be assigned a number to.\n"
                      "Often used by solve() and algsys(), if there is an infinite number of results."));
    }

  
    if(m_text.StartsWith("%i"))
    {
      wxString number;

      number = m_text.Right(m_text.Length()-2);

      bool isinum = (number != wxEmptyString);
     
      for (wxString::const_iterator it = number.begin(); it != number.end(); ++it)
        if(!wxIsdigit(*it))
        {
          isinum = false;
          break;
        }
      
      if(isinum)
        SetToolTip( _("An integration constant."));
    }
  }
  
  if (m_textStyle == TS_NUMBER)
  {
    m_numStart = wxEmptyString;
    m_numEnd = wxEmptyString;
    m_ellipsis = wxEmptyString;
    unsigned int displayedDigits = (*m_configuration)->GetDisplayedDigits();
    if (m_displayedText.Length() > displayedDigits)
    {
      int left = displayedDigits / 3;
      if (left > 30) left = 30;      
      m_numStart = m_displayedText.Left(left);
      m_ellipsis = wxString::Format(_("[%i digits]"), (int) m_displayedText.Length() - 2 * left);
      m_numEnd = m_displayedText.Right(left);
    }
    else
    {
      m_numStart = wxEmptyString;
      m_numEnd = wxEmptyString;
      m_ellipsis = wxEmptyString;
      if(
        (m_roundingErrorRegEx1.Matches(m_displayedText)) ||
        (m_roundingErrorRegEx2.Matches(m_displayedText)) ||
        (m_roundingErrorRegEx3.Matches(m_displayedText)) ||
        (m_roundingErrorRegEx4.Matches(m_displayedText))
        )
        SetToolTip( _("As calculating 0.1^12 demonstrates maxima by default doesn't tend to "
                      "hide what looks like being the small error using floating-point "
                      "numbers introduces.\n"
                      "If this seems to be the case here the error can be avoided by using "
                      "exact numbers like 1/10, 1*10^-1 or rat(.1).\n"
                      "It also can be hidden by setting fpprintprec to an appropriate value. "
                      "But be aware in this case that even small errors can add up."));
    }
  }
  else
  {
    if((text.Contains(wxT("LINE SEARCH FAILED. SEE")))||
       (text.Contains(wxT("DOCUMENTATION OF ROUTINE MCSRCH"))) ||
       (text.Contains(wxT("ERROR RETURN OF LINE SEARCH:"))) ||
       text.Contains(wxT("POSSIBLE CAUSES: FUNCTION OR GRADIENT ARE INCORRECT")))
      SetToolTip( _("This message can appear when trying to numerically find an optimum. "
                    "In this case it might indicate that a starting point lies in a local "
                    "optimum that fits the data best if one parameter is increased to "
                    "infinity or decreased to -infinity. It also can indicate that an "
                    "attempt was made to fit data to an equation that actually matches "
                    "the data best if one parameter is set to +/- infinity."));
    if(text.StartsWith(wxT("incorrect syntax")) && (text.Contains(wxT("is not an infix operator"))))
      SetToolTip( _("A command or number wasn't preceded by a \":\", a \"$\", a \";\" or a \",\".\n"
                    "Most probable cause: A missing comma between two list items."));
    if(text.StartsWith(wxT("incorrect syntax")) && (text.Contains(wxT("Found LOGICAL expression where ALGEBRAIC expression expected"))))
      SetToolTip( _("Most probable cause: A dot instead a comma between two list items containing assignments."));
    if(text.StartsWith(wxT("incorrect syntax")) && (text.Contains(wxT("is not a prefix operator"))))
      SetToolTip( _("Most probable cause: Two commas or similar separators in a row."));
    if(text.Contains(wxT("Illegal use of delimiter")))
      SetToolTip( _("Most probable cause: an operator was directly followed by a closing parenthesis."));
    
    if(text.StartsWith(wxT("part: fell off the end.")))
      SetToolTip( _("part() or the [] operator was used in order to extract the nth element "
                    "of something that was less than n elements long."));
    if(text.StartsWith(wxT("rest: fell off the end.")))
      SetToolTip( _("rest() tried to drop more entries from a list than the list was long."));
    if(text.StartsWith(wxT("assignment: cannot assign to")))
      SetToolTip( _("The value of few special variables is assigned by Maxima and cannot be changed by the user. Also a few constructs aren't variable names and therefore cannot be written to."));
    if(text.StartsWith(wxT("rat: replaced ")))
      SetToolTip( _("Normally computers use floating-point numbers that can be handled "
                    "incredibly fast while being accurate to dozens of digits. "
                    "They will, though, introduce a small error into some common numbers. "
                    "For example 0.1 is represented as 3602879701896397/36028797018963968.\n"
                    "As mathematics is based on the fact that numbers that are exactly "
                    "equal cancel each other out small errors can quickly add up to big errors "
                    "(see Wilkinson's Polynomials or Rump's Polynomials). Some maxima "
                    "commands therefore use rat() in order to automatically convert floats to "
                    "exact numbers (like 1/10 or sqrt(2)/2) where floating-point errors might "
                    "add up.\n\n"
                    "This error message doesn't occur if exact numbers (1/10 instead of 0.1) "
                    "are used.\n"
                    "The info that numbers have automatically been converted can be suppressed "
                    "by setting ratprint to false."));
    if(text.StartsWith("desolve: can't handle this case."))
      SetToolTip( _("The list of time-dependent variables to solve to doesn't match the time-dependent variables the list of dgls contains."));      
    if(text.StartsWith(wxT("expt: undefined: 0 to a negative exponent.")))
      SetToolTip( _("Division by 0."));
    if(text.StartsWith(wxT("incorrect syntax: parser: incomplete number; missing exponent?")))
      SetToolTip( _("Might also indicate a missing multiplication sign (\"*\")."));
    if(text.Contains(wxT("arithmetic error DIVISION-BY-ZERO signalled")))
      SetToolTip( _("Besides a division by 0 the reason for this error message can be a "
                    "calculation that returns +/-infinity."));
    if(text.Contains(wxT("isn't in the domain of")))
      SetToolTip( _("Most probable cause: A function was called with a parameter that causes "
                    "it to return infinity and/or -infinity."));
    if(text.StartsWith(wxT("Only symbols can be bound")))
      SetToolTip( _("This error message is most probably caused by a try to assign "
                    "a value to a number instead of a variable name.\n"
                    "One probable cause is using a variable that already has a numeric "
                    "value as a loop counter."));
    if(text.StartsWith(wxT("append: operators of arguments must all be the same.")))
      SetToolTip( _("Most probably it was attempted to append something to a list "
                    "that isn't a list.\n"
                    "Enclosing the new element for the list in brackets ([]) "
                    "converts it to a list and makes it appendable."));
    if(text.Contains(wxT(": invalid index")))
      SetToolTip( _("The [] or the part() command tried to access a list or matrix "
                    "element that doesn't exist."));
    if(text.StartsWith(wxT("apply: subscript must be an integer; found:")))
      SetToolTip( _("the [] operator tried to extract an element of a list, a matrix, "
                    "an equation or an array. But instead of an integer number "
                    "something was used whose numerical value is unknown or not an "
                    "integer.\n"
                    "Floating-point numbers are bound to contain small rounding errors "
                    "and therefore in most cases don't work as an array index that"
                    "needs to be an exact integer number."));
    if(text.StartsWith(wxT(": improper argument: ")))
    {
      if((m_previous) && (m_previous->ToString() == wxT("at")))
        SetToolTip( _("The second argument of at() isn't an equation or a list of "
                      "equations. Most probably it was lacking an \"=\"."));
      else if((m_previous) && (m_previous->ToString() == wxT("subst")))
        SetToolTip( _("The first argument of subst() isn't an equation or a list of "
                      "equations. Most probably it was lacking an \"=\"."));
      else
        SetToolTip( _("The argument of a function was of the wrong type. Most probably "
                      "an equation was expected but was lacking an \"=\"."));
    }
  }
  m_alt = m_altJs = false;
  ResetSize();
}

// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_altText
// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_altJsText
// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_fontname
// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_texFontname
// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_alt
// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_altJs
// cppcheck-suppress uninitMemberVar symbolName=TextCell::m_initialToolTip
TextCell::TextCell(const TextCell &cell):
  Cell(cell.m_group, cell.m_configuration, cell.m_cellPointers),
  m_text(cell.m_text),
  m_userDefinedLabel(cell.m_userDefinedLabel),
  m_displayedText(cell.m_displayedText)
{
  CopyCommonData(cell);
  m_forceBreakLine = cell.m_forceBreakLine;
  m_bigSkip = cell.m_bigSkip;
  m_lastZoomFactor = -1;
  m_fontSizeLabel = -1;
  m_displayedDigits_old = -1;
  m_lastCalculationFontSize = -1;
  m_realCenter = -1;
  m_fontsize_old = -1;
  m_textStyle = cell.m_textStyle;
  m_highlight = cell.m_highlight;
  m_dontEscapeOpeningParenthesis = cell.m_dontEscapeOpeningParenthesis;
}

wxSize TextCell::GetTextSize(wxString const &text)
{
  wxDC *dc = (*m_configuration)->GetDC();
  double fontSize = dc->GetFont().GetPointSize();

  SizeHash::const_iterator it = m_widths.find(fontSize);

  // If we already know this text piece's size we return the cached value
  if(it != m_widths.end())
    return it->second;

  // Ask wxWidgets to return this text piece's size (slow!)
  wxSize sz = dc->GetTextExtent(text);
  m_widths[fontSize] = sz;
  return sz;
}

bool TextCell::NeedsRecalculation()
{
  return Cell::NeedsRecalculation() ||
    (
      (m_textStyle == TS_USERLABEL) &&
      (!(*m_configuration)->UseUserLabels())
      ) ||
    (
      (m_textStyle == TS_LABEL) &&
      ((*m_configuration)->UseUserLabels()) &&
    (m_userDefinedLabel != wxEmptyString)
      ) ||
    (
      (m_textStyle == TS_NUMBER) &&
      (m_displayedDigits_old != (*m_configuration)->GetDisplayedDigits())
      );
}

void TextCell::RecalculateWidths(int fontsize)
{
  if(fontsize < 1)
    fontsize = m_fontSize;
  if(fontsize != m_fontsize_old)
    ResetSize();
  Configuration *configuration = (*m_configuration);
  
  if(NeedsRecalculation())
  {      
    m_fontSize = m_fontsize_old = fontsize;
    wxDC *dc = configuration->GetDC();
    SetFont(fontsize);

    // If the setting has changed and we want to show a user-defined label
    // instead of an automatic one or vice versa we decide that here.
    if(
      (m_textStyle == TS_USERLABEL) &&
      (!configuration->UseUserLabels())
      )
      m_textStyle = TS_LABEL;
    if(
      (m_textStyle == TS_LABEL) &&
      (configuration->UseUserLabels()) &&
      (m_userDefinedLabel != wxEmptyString)
      )
      m_textStyle = TS_USERLABEL;
    
    SetAltText();
    
    // If the config settings about how many digits to display has changed we
    // need to regenerate the info which number to show.
    if (
      (m_textStyle == TS_NUMBER) &&
      (m_displayedDigits_old != (*m_configuration)->GetDisplayedDigits())
        )
    {
      SetValue(m_text);
      m_numstartWidths.clear();
      m_ellipsisWidths.clear();
      m_numEndWidths.clear();
    }
    
    m_lastCalculationFontSize = fontsize;


    if(m_numStart != wxEmptyString)
    {      
      double fontSize = dc->GetFont().GetPointSize();
      {
        SizeHash::const_iterator it = m_numstartWidths.find(fontSize);    
        if(it != m_numstartWidths.end())
          m_numStartWidth = it->second;
        else
        {
          wxSize sz = dc->GetTextExtent(m_numStart);
          m_numstartWidths[fontSize] = sz;
          m_numStartWidth = sz;
        }
      }
      {
        SizeHash::const_iterator it = m_numEndWidths.find(fontSize);    
        if(it != m_numEndWidths.end())
          m_numEndWidth = it->second;
        else
        {
          wxSize sz = dc->GetTextExtent(m_numEnd);
          m_numEndWidths[fontSize] = sz;
          m_numEndWidth = sz;
        }
      }
      {
        SizeHash::const_iterator it = m_ellipsisWidths.find(fontSize);    
        if(it != m_ellipsisWidths.end())
          m_ellipsisWidth = it->second;
        else
        {
          wxSize sz = dc->GetTextExtent(m_ellipsis);
          m_ellipsisWidths[fontSize] = sz;
          m_ellipsisWidth = sz;
        }
      }
      m_width = m_numStartWidth.GetWidth() + m_numEndWidth.GetWidth() +
        m_ellipsisWidth.GetWidth();
      m_height = wxMax(
        wxMax(m_numStartWidth.GetHeight(), m_numEndWidth.GetHeight()),
        m_ellipsisWidth.GetHeight());
    }
    else
    {    
      // Labels and prompts are fixed width - adjust font size so that
      // they fit in
      if ((m_textStyle == TS_LABEL) || (m_textStyle == TS_USERLABEL) || (m_textStyle == TS_MAIN_PROMPT))
      {
        wxString text = m_text;

        if(m_textStyle == TS_USERLABEL)
        {
          text = wxT("(") + m_userDefinedLabel + wxT(")");
          m_unescapeRegEx.ReplaceAll(&text,wxT("\\1"));
        }

        wxFont font = dc->GetFont();
        double fontsize1 = Scale_Px(configuration->GetDefaultFontSize());
        if(fontsize1 < 4)
          fontsize1 = 4;
#if wxCHECK_VERSION(3, 1, 2)
        font.SetFractionalPointSize(fontsize1);
#else
        font.SetPointSize(fontsize1);
#endif
        dc->SetFont(font);

      
        m_width = Scale_Px(configuration->GetLabelWidth());
        // We will decrease it before use
        m_fontSizeLabel = m_fontSize + 1;
        wxSize labelSize = GetTextSize(text);
        wxASSERT_MSG((labelSize.GetWidth() > 0) || (m_displayedText == wxEmptyString),
                     _("Seems like something is broken with the maths font. Installing http://www.math.union.edu/~dpvc/jsmath/download/jsMath-fonts.html and checking \"Use JSmath fonts\" in the configuration dialogue should fix it."));
        font = dc->GetFont();
#if wxCHECK_VERSION(3, 1, 2)
        font.SetFractionalPointSize(Scale_Px(m_fontSizeLabel));
#else
        font.SetPointSize(Scale_Px(m_fontSizeLabel));
#endif
        labelSize = GetTextSize(text);
        while ((labelSize.GetWidth() >= m_width) && (m_fontSizeLabel > 2))
        {
#if wxCHECK_VERSION(3, 1, 2)
          m_fontSizeLabel -= .3 + 3 * (m_width - labelSize.GetWidth()) / labelSize.GetWidth() / 4;
          font.SetFractionalPointSize(Scale_Px(m_fontSizeLabel));
#else
          m_fontSizeLabel -= 1 + 3 * (m_width - labelSize.GetWidth()) / labelSize.GetWidth() / 4;
          font.SetPointSize(Scale_Px(m_fontSizeLabel));
#endif
          dc->SetFont(font);
          labelSize = GetTextSize(text);
        } 
        m_width = wxMax(m_width + MC_TEXT_PADDING, Scale_Px(configuration->GetLabelWidth()) + MC_TEXT_PADDING);
        m_height = labelSize.GetHeight();
        m_center = m_height / 2;
      }
      // Check if we are using jsMath and have jsMath character
      else if (m_altJs && configuration->CheckTeXFonts())
      {      
        wxSize sz = GetTextSize(m_altJsText);
        m_width = sz.GetWidth();
        m_height = sz.GetHeight();
        if (m_texFontname == wxT("jsMath-cmsy10"))
          m_height = m_height / 2;
      }

      /// We are using a special symbol
      else if (m_alt)
      {
        wxSize sz = GetTextSize(m_altText);
        m_width = sz.GetWidth();
        m_height = sz.GetHeight();
      }
      else if (m_displayedText.IsEmpty())
      {
        m_height = m_fontSize;
        m_width = 0;
      }

      /// This is the default.
      else
      {
        wxSize sz = GetTextSize(m_displayedText);
        m_width = sz.GetWidth();
        m_height = sz.GetHeight();
      }
    
      m_width += 2 * MC_TEXT_PADDING;
      m_height += 2 * MC_TEXT_PADDING;

      /// Hidden cells (multiplication * is not displayed)
      if ((m_isHidden) || ((configuration->HidemultiplicationSign()) && m_isHidableMultSign))
      {
        m_height = 0;
        m_width = m_width / 4;
      }
    }
  }
  if(m_height < Scale_Px(4)) m_height = Scale_Px(4);
  m_realCenter = m_center = m_height / 2;
  Cell::RecalculateWidths(fontsize);
}

void TextCell::Draw(wxPoint point)
{
  Cell::Draw(point);
  Configuration *configuration = (*m_configuration);
  if (DrawThisCell(point) &&
      !(m_isHidden || ((configuration->HidemultiplicationSign()) && m_isHidableMultSign)))
  {
    wxDC *dc = configuration->GetDC();
    
    if (NeedsRecalculation())
      RecalculateWidths(m_fontSize);
    
    if (InUpdateRegion())
    {
      SetFont(m_fontSize);
      SetForeground();
      /// Labels and prompts have special fontsize
      if ((m_textStyle == TS_LABEL) || (m_textStyle == TS_USERLABEL) || (m_textStyle == TS_MAIN_PROMPT))
      {
        if ((m_textStyle == TS_USERLABEL || configuration->ShowAutomaticLabels()) &&
            configuration->ShowLabels())
        {
          SetFontSizeForLabel(dc);
          // Draw the label
          if(m_textStyle == TS_USERLABEL)
          {
            wxString text = m_userDefinedLabel;
            SetToolTip(m_text);
            m_unescapeRegEx.ReplaceAll(&text,wxT("\\1"));
            dc->DrawText(wxT("(") + text + wxT(")"),
                         point.x + MC_TEXT_PADDING,
                         point.y - m_realCenter + MC_TEXT_PADDING);
          }
          else
          {
            SetToolTip(m_userDefinedLabel);
            dc->DrawText(m_displayedText,
                         point.x + MC_TEXT_PADDING,
                         point.y - m_realCenter + MC_TEXT_PADDING);
          }
        }
      }
      else if (!m_numStart.IsEmpty())
      {
        dc->DrawText(m_numStart,
                     point.x + MC_TEXT_PADDING,
                     point.y - m_realCenter + MC_TEXT_PADDING);
        dc->DrawText(m_numEnd,
                     point.x + MC_TEXT_PADDING + m_numStartWidth.GetWidth() +
                     m_ellipsisWidth.GetWidth(),
                     point.y - m_realCenter + MC_TEXT_PADDING);
        wxColor textColor = dc->GetTextForeground();
        wxColor backgroundColor = dc->GetTextBackground();
        dc->SetTextForeground(
          wxColor(
            (textColor.Red() + backgroundColor.Red()) / 2,
            (textColor.Green() + backgroundColor.Green()) / 2,
            (textColor.Blue() + backgroundColor.Blue()) / 2
            )
          );
        dc->DrawText(m_ellipsis,
                     point.x + MC_TEXT_PADDING + m_numStartWidth.GetWidth(),
                     point.y - m_realCenter + MC_TEXT_PADDING);
      }
        /// Check if we are using jsMath and have jsMath character
      else if (m_altJs && configuration->CheckTeXFonts())
        dc->DrawText(m_altJsText,
                    point.x + MC_TEXT_PADDING,
                    point.y - m_realCenter + MC_TEXT_PADDING);

        /// We are using a special symbol
      else if (m_alt)
        dc->DrawText(m_altText,
                    point.x + MC_TEXT_PADDING,
                    point.y - m_realCenter + MC_TEXT_PADDING);

        /// Change asterisk
      else if (configuration->GetChangeAsterisk() && m_displayedText == wxT("*"))
        dc->DrawText(wxT("\u00B7"),
                    point.x + MC_TEXT_PADDING,
                    point.y - m_realCenter + MC_TEXT_PADDING);

      else if (m_displayedText == wxT("#"))
        dc->DrawText(wxT("\u2260"),
                    point.x + MC_TEXT_PADDING,
                    point.y - m_realCenter + MC_TEXT_PADDING);
        /// This is the default.
      else
      {
        switch (GetType())
        {
          case MC_TYPE_TEXT:
            // TODO: Add markdown formatting for bold, italic and underlined here.
            dc->DrawText(m_displayedText,
                        point.x + MC_TEXT_PADDING,
                        point.y - m_realCenter + MC_TEXT_PADDING);
            break;
          case MC_TYPE_INPUT:
            // This cell has already been drawn as an EditorCell => we don't repeat this action here.
            break;
          default:
            dc->DrawText(m_displayedText,
                        point.x + MC_TEXT_PADDING,
                        point.y - m_realCenter + MC_TEXT_PADDING);
        }
      }
    }
  }
}

void TextCell::SetFontSizeForLabel(wxDC *dc)
{
  wxFont font(dc->GetFont());
#if wxCHECK_VERSION(3, 1, 2)
  font.SetFractionalPointSize(Scale_Px(m_fontSizeLabel));
#else
  font.SetPointSize(Scale_Px(m_fontSizeLabel));
#endif
  dc->SetFont(font);
}

void TextCell::SetFont(int fontsize)
{
  Configuration *configuration = (*m_configuration);
  wxDC *dc = configuration->GetDC();

  if ((m_textStyle == TS_TITLE) ||
      (m_textStyle == TS_SECTION) ||
      (m_textStyle == TS_SUBSECTION) ||
      (m_textStyle == TS_SUBSUBSECTION) ||
      (m_textStyle == TS_HEADING5) || 
      (m_textStyle == TS_HEADING6))
  {
    // Titles have a fixed font size 
    m_fontSize = configuration->GetFontSize(m_textStyle);
  }
  else
  {
    // Font within maths has a dynamic font size that might be reduced for example
    // within fractions, subscripts or superscripts.
    if (
      (m_textStyle != TS_MAIN_PROMPT) &&
      (m_textStyle != TS_OTHER_PROMPT) &&
      (m_textStyle != TS_ERROR) &&
      (m_textStyle != TS_WARNING)
      )
      m_fontSize = fontsize;
  }
    
  wxFont font = configuration->GetFont(m_textStyle,fontsize);

  // Use jsMath
  if (m_altJs && configuration->CheckTeXFonts())
    font.SetFaceName(m_texFontname);
  
  if (!font.IsOk())
  {
    font.SetFamily(wxFONTFAMILY_MODERN);
    font.SetFaceName(wxEmptyString);
  }
  
  if (!font.IsOk())
    font = *wxNORMAL_FONT;

  if(m_fontSize < 4)
    m_fontSize = 4;
  
  // Mark special variables that are printed as ordinary letters as being special.
  if ((!(*m_configuration)->CheckKeepPercent()) &&
      ((m_text == wxT("%e")) || (m_text == wxT("%i"))))
  {
    if((*m_configuration)->IsItalic(TS_VARIABLE) != wxFONTSTYLE_NORMAL)
    {
      font.SetStyle(wxFONTSTYLE_NORMAL);
    }
    else
    {
      font.SetStyle(wxFONTSTYLE_ITALIC);
    }
  }

  wxASSERT(Scale_Px(m_fontSize) > 0);
#if wxCHECK_VERSION(3, 1, 2)
  font.SetFractionalPointSize(Scale_Px(m_fontSize));
#else
  font.SetPointSize(Scale_Px(m_fontSize));
#endif

  wxASSERT_MSG(font.IsOk(),
               _("Seems like something is broken with a font. Installing http://www.math.union.edu/~dpvc/jsmath/download/jsMath-fonts.html and checking \"Use JSmath fonts\" in the configuration dialogue should fix it."));
  dc->SetFont(font);
  
  // A fallback if we have been completely unable to set a working font
  if (!dc->GetFont().IsOk())
  {
    dc->SetFont(wxFontInfo(10));
  }
}

bool TextCell::IsOperator() const
{
  if (wxString(wxT("+*/-")).Find(m_text) >= 0)
    return true;
  if (m_text == wxT("\u2212"))
    return true;
  return false;
}

wxString TextCell::ToString()
{
  wxString text;
  if (m_altCopyText != wxEmptyString)
    text = m_altCopyText;
  else
  {
    text = m_text;
    if(((*m_configuration)->UseUserLabels())&&(m_userDefinedLabel != wxEmptyString))
      text = wxT("(") + m_userDefinedLabel + wxT(")");
    text.Replace(wxT("\u2212"), wxT("-")); // unicode minus sign
    text.Replace(wxT("\u2794"), wxT("-->"));
    text.Replace(wxT("\u2192"), wxT("->"));
  }
  switch (m_textStyle)
  {
    case TS_VARIABLE:
    case TS_FUNCTION:
      // The only way for variable or function names to contain quotes and
      // characters that clearly represent operators is that these chars
      // are quoted by a backslash: They cannot be quoted by quotation
      // marks since maxima would'nt allow strings here.
    {
      wxString charsNeedingQuotes("\\'\"()[]-{}^+*/&§?:;=#<>$");
      bool isOperator = true;
      if(m_text.Length() > 1)
      {
        for (size_t i = 0; i < m_text.Length(); i++)
        {
          if ((m_text[i] == wxT(' ')) || (charsNeedingQuotes.Find(m_text[i]) == wxNOT_FOUND))
          {
            isOperator = false;
            break;
          }
        }
      }

      if (!isOperator)
      {
        wxString lastChar;
        if ((m_dontEscapeOpeningParenthesis) && (text.Length() > 0) && (text[text.Length() - 1] == wxT('(')))
        {
          lastChar = text[text.Length() - 1];
          text = text.Left(text.Length() - 1);
        }
        for (size_t i = 0; i < charsNeedingQuotes.Length(); i++)
          text.Replace(charsNeedingQuotes[i], wxT("\\") + wxString(charsNeedingQuotes[i]));
        text += lastChar;
      }
      break;
    }
    case TS_STRING:
      text = wxT("\"") + text + wxT("\"");
      break;

      // Labels sometimes end with a few spaces. But if they are long they don't do
      // that any more => Add a TAB to the end of any label replacing trailing
      // whitespace. But don't do this if we copy only the label.
    case TS_LABEL:
    case TS_USERLABEL:
    case TS_MAIN_PROMPT:
    case TS_OTHER_PROMPT:
      {
        text.Trim();
        text += wxT("\t");
        break;
      }
  default:
  {}
  }
  if((m_next != NULL) && (m_next->BreakLineHere()))
    text += "\n";
  
  return text;
}

wxString TextCell::ToMatlab()
{
	wxString text;
	if (m_altCopyText != wxEmptyString)
	  text = m_altCopyText;
	else
	{
	  text = m_text;
	  if(((*m_configuration)->UseUserLabels())&&(m_userDefinedLabel != wxEmptyString))
		text = wxT("(") + m_userDefinedLabel + wxT(")");
	  text.Replace(wxT("\u2212"), wxT("-")); // unicode minus sign
	  text.Replace(wxT("\u2794"), wxT("-->"));
	  text.Replace(wxT("\u2192"), wxT("->"));

	  if (text == wxT("%e"))
		text = wxT("e");
	  else if (text == wxT("%i"))
		text = wxT("i");
	  else if (text == wxT("%pi"))
		text = wxString(wxT("pi"));
	}
	switch (m_textStyle)
	{
	  case TS_VARIABLE:
	  case TS_FUNCTION:
		// The only way for variable or function names to contain quotes and
		// characters that clearly represent operators is that these chars
		// are quoted by a backslash: They cannot be quoted by quotation
		// marks since maxima would'nt allow strings here.
	  {
		wxString charsNeedingQuotes("\\'\"()[]{}^+*/&§?:;=#<>$");
		bool isOperator = true;
		for (size_t i = 0; i < m_text.Length(); i++)
		{
		  if ((m_text[i] == wxT(' ')) || (charsNeedingQuotes.Find(m_text[i]) == wxNOT_FOUND))
		  {
			isOperator = false;
			break;
		  }
		}

		if (!isOperator)
		{
		  wxString lastChar;
		  if ((m_dontEscapeOpeningParenthesis) && (text.Length() > 0) && (text[text.Length() - 1] == wxT('(')))
		  {
			lastChar = text[text.Length() - 1];
			text = text.Left(text.Length() - 1);
		  }
		  for (size_t i = 0; i < charsNeedingQuotes.Length(); i++)
			text.Replace(charsNeedingQuotes[i], wxT("\\") + wxString(charsNeedingQuotes[i]));
		  text += lastChar;
		}
		break;
	  }
	  case TS_STRING:
		text = wxT("\"") + text + wxT("\"");
		break;

		// Labels sometimes end with a few spaces. But if they are long they don't do
		// that any more => Add a TAB to the end of any label replacing trailing
		// whitespace. But don't do this if we copy only the label.
	  case TS_LABEL:
	  case TS_USERLABEL:
	  case TS_MAIN_PROMPT:
	  case TS_OTHER_PROMPT:
		{
		  text.Trim();
		  text += wxT("\t");
		  break;
		}
	default:
	{}
	}
	if((m_next != NULL) && (m_next->BreakLineHere()))
	  text += "\n";

	return text;
}

wxString TextCell::ToTeX()
{
  wxString text = m_displayedText;

  if(((*m_configuration)->UseUserLabels())&&(m_userDefinedLabel != wxEmptyString))
    text = wxT("(") + m_userDefinedLabel + wxT(")");

  if (!(*m_configuration)->CheckKeepPercent())
  {
    if (text == wxT("%e"))
      text = wxT("e");
    else if (text == wxT("%i"))
      text = wxT("i");
    else if (text == wxT("%pi"))
      text = wxString(wxT("\u03C0"));
  }

  // The string needed in order to ensure we are in math mode. Most TextCells contain names of
  // math objects and therefore can leave this string blank.
  wxString mathModeStart;
  // The string needed in order to close the command that ensures we are in math mode.
  wxString mathModeEnd = wxT(" ");

  if (
          (GetStyle() == TS_ERROR) ||
          (GetStyle() == TS_WARNING) ||
          (GetStyle() == TS_LABEL) ||
          (GetStyle() == TS_USERLABEL) ||
          (GetStyle() == TS_MAIN_PROMPT) ||
          (GetStyle() == TS_OTHER_PROMPT)
          )
  {
    mathModeStart = wxT("\\ensuremath{");
    mathModeEnd = wxT("}");
    text.Replace(wxT("\\"), mathModeStart + wxT("\\backslash") + mathModeEnd);
    text.Replace(wxT("{"), wxT("\\{"));
    text.Replace(wxT("}"), wxT("\\}"));
  }
  else
  {
    text.Replace(wxT("\\"), mathModeStart + wxT("\\backslash") + mathModeEnd);
    text.Replace(wxT("{"), wxT("\\{"));
    text.Replace(wxT("}"), wxT("\\}"));

    // Babel replaces Umlaute by constructs like \"a - and \" isn't allowed in
    // math mode. Fortunately amsTeX provides the \text command that allows to
    // switch to plain text mode again - but with the math font size.
    text.Replace(wxT("ä"), wxT("\\text{ä}"));
    text.Replace(wxT("ö"), wxT("\\text{ö}"));
    text.Replace(wxT("ü"), wxT("\\text{ü}"));
    text.Replace(wxT("Ä"), wxT("\\text{Ä}"));
    text.Replace(wxT("Ö"), wxT("\\text{Ö}"));
    text.Replace(wxT("Ü"), wxT("\\text{Ü}"));
  }
  
  text.Replace(wxT("<"), mathModeStart + wxT("<") + mathModeEnd);
  text.Replace(wxT(">"), mathModeStart + wxT(">") + mathModeEnd);
  text.Replace(wxT("\u2212"), wxT("-")); // unicode minus sign
  text.Replace(wxT("\u00B1"), mathModeStart + wxT("\\pm") + mathModeEnd);
  text.Replace(wxT("\u03B1"), mathModeStart + wxT("\\alpha") + mathModeEnd);
  text.Replace(wxT("\u00B2"), mathModeStart + wxT("^2") + mathModeEnd);
  text.Replace(wxT("\u00B3"), mathModeStart + wxT("^3") + mathModeEnd);
  text.Replace(wxT("\u221A"), mathModeStart + wxT("\\sqrt{}") + mathModeEnd);
  text.Replace(wxT("\u2148"), mathModeStart + wxT("\\mathbbm{i}") + mathModeEnd);
  text.Replace(wxT("\u2147"), mathModeStart + wxT("\\mathbbm{e}") + mathModeEnd);
  text.Replace(wxT("\u210f"), mathModeStart + wxT("\\hbar") + mathModeEnd);
  text.Replace(wxT("\u2203"), mathModeStart + wxT("\\exists") + mathModeEnd);
  text.Replace(wxT("\u2204"), mathModeStart + wxT("\\nexists") + mathModeEnd);
  text.Replace(wxT("\u2208"), mathModeStart + wxT("\\in") + mathModeEnd);
  text.Replace(wxT("\u21D2"), mathModeStart + wxT("\\Longrightarrow") + mathModeEnd);
  text.Replace(wxT("\u221e"), mathModeStart + wxT("\\infty") + mathModeEnd);
  text.Replace(wxT("\u22C0"), mathModeStart + wxT("\\wedge") + mathModeEnd);
  text.Replace(wxT("\u22C1"), mathModeStart + wxT("\\vee") + mathModeEnd);
  text.Replace(wxT("\u22bb"), mathModeStart + wxT("\\oplus") + mathModeEnd);
  text.Replace(wxT("\u22BC"), mathModeStart + wxT("\\overline{\\wedge}") + mathModeEnd);
  text.Replace(wxT("\u22BB"), mathModeStart + wxT("\\overline{\\vee}") + mathModeEnd);
  text.Replace(wxT("\u00AC"), mathModeStart + wxT("\\setminus") + mathModeEnd);
  text.Replace(wxT("\u22C3"), mathModeStart + wxT("\\cup") + mathModeEnd);
  text.Replace(wxT("\u22C2"), mathModeStart + wxT("\\cap") + mathModeEnd);
  text.Replace(wxT("\u2286"), mathModeStart + wxT("\\subseteq") + mathModeEnd);
  text.Replace(wxT("\u2282"), mathModeStart + wxT("\\subset") + mathModeEnd);
  text.Replace(wxT("\u2288"), mathModeStart + wxT("\\not\\subseteq") + mathModeEnd);
  text.Replace(wxT("\u0127"), mathModeStart + wxT("\\hbar") + mathModeEnd);
  text.Replace(wxT("\u0126"), mathModeStart + wxT("\\Hbar") + mathModeEnd);
  text.Replace(wxT("\u2205"), mathModeStart + wxT("\\emptyset") + mathModeEnd);
  text.Replace(wxT("\u00BD"), mathModeStart + wxT("\\frac{1}{2}") + mathModeEnd);
  text.Replace(wxT("\u03B2"), mathModeStart + wxT("\\beta") + mathModeEnd);
  text.Replace(wxT("\u03B3"), mathModeStart + wxT("\\gamma") + mathModeEnd);
  text.Replace(wxT("\u03B4"), mathModeStart + wxT("\\delta") + mathModeEnd);
  text.Replace(wxT("\u03B5"), mathModeStart + wxT("\\epsilon") + mathModeEnd);
  text.Replace(wxT("\u03B6"), mathModeStart + wxT("\\zeta") + mathModeEnd);
  text.Replace(wxT("\u03B7"), mathModeStart + wxT("\\eta") + mathModeEnd);
  text.Replace(wxT("\u03B8"), mathModeStart + wxT("\\theta") + mathModeEnd);
  text.Replace(wxT("\u03B9"), mathModeStart + wxT("\\iota") + mathModeEnd);
  text.Replace(wxT("\u03BA"), mathModeStart + wxT("\\kappa") + mathModeEnd);
  text.Replace(wxT("\u03BB"), mathModeStart + wxT("\\lambda") + mathModeEnd);
  text.Replace(wxT("\u03BC"), mathModeStart + wxT("\\mu") + mathModeEnd);
  text.Replace(wxT("\u03BD"), mathModeStart + wxT("\\nu") + mathModeEnd);
  text.Replace(wxT("\u03BE"), mathModeStart + wxT("\\xi") + mathModeEnd);
  text.Replace(wxT("\u03BF"), wxT("o"));
  text.Replace(wxT("\u03C0"), mathModeStart + wxT("\\pi") + mathModeEnd);
  text.Replace(wxT("\u03C1"), mathModeStart + wxT("\\rho") + mathModeEnd);
  text.Replace(wxT("\u03C3"), mathModeStart + wxT("\\sigma") + mathModeEnd);
  text.Replace(wxT("\u03C4"), mathModeStart + wxT("\\tau") + mathModeEnd);
  text.Replace(wxT("\u03C5"), mathModeStart + wxT("\\upsilon") + mathModeEnd);
  text.Replace(wxT("\u03C6"), mathModeStart + wxT("\\phi") + mathModeEnd);
  text.Replace(wxT("\u03C7"), mathModeStart + wxT("\\chi") + mathModeEnd);
  text.Replace(wxT("\u03C8"), mathModeStart + wxT("\\psi") + mathModeEnd);
  text.Replace(wxT("\u03C9"), mathModeStart + wxT("\\omega") + mathModeEnd);
  text.Replace(wxT("\u0391"), wxT("A"));
  text.Replace(wxT("\u0392"), wxT("B"));
  text.Replace(wxT("\u0393"), mathModeStart + wxT("\\Gamma") + mathModeEnd);
  text.Replace(wxT("\u0394"), mathModeStart + wxT("\\Delta") + mathModeEnd);
  text.Replace(wxT("\u0395"), wxT("E"));
  text.Replace(wxT("\u0396"), wxT("Z"));
  text.Replace(wxT("\u0397"), wxT("H"));
  text.Replace(wxT("\u0398"), mathModeStart + wxT("\\Theta") + mathModeEnd);
  text.Replace(wxT("\u0399"), wxT("I"));
  text.Replace(wxT("\u039A"), wxT("K"));
  text.Replace(wxT("\u039B"), mathModeStart + wxT("\\Lambda") + mathModeEnd);
  text.Replace(wxT("\u039C"), wxT("M"));
  text.Replace(wxT("\u039D"), wxT("N"));
  text.Replace(wxT("\u039E"), mathModeStart + wxT("\\Xi") + mathModeEnd);
  text.Replace(wxT("\u039F"), wxT("O"));
  text.Replace(wxT("\u03A0"), mathModeStart + wxT("\\Pi") + mathModeEnd);
  text.Replace(wxT("\u03A1"), wxT("P"));
  text.Replace(wxT("\u03A3"), mathModeStart + wxT("\\Sigma") + mathModeEnd);
  text.Replace(wxT("\u03A4"), wxT("T"));
  text.Replace(wxT("\u03A5"), mathModeStart + wxT("\\Upsilon") + mathModeEnd);
  text.Replace(wxT("\u03A6"), mathModeStart + wxT("\\Phi") + mathModeEnd);
  text.Replace(wxT("\u03A7"), wxT("X"));
  text.Replace(wxT("\u03A8"), mathModeStart + wxT("\\Psi") + mathModeEnd);
  text.Replace(wxT("\u03A9"), mathModeStart + wxT("\\Omega") + mathModeEnd);
  text.Replace(wxT("\u2202"), mathModeStart + wxT("\\partial") + mathModeEnd);
  text.Replace(wxT("\u222b"), mathModeStart + wxT("\\int") + mathModeEnd);
  text.Replace(wxT("\u2245"), mathModeStart + wxT("\\approx") + mathModeEnd);
  text.Replace(wxT("\u221d"), mathModeStart + wxT("\\propto") + mathModeEnd);
  text.Replace(wxT("\u2260"), mathModeStart + wxT("\\neq") + mathModeEnd);
  text.Replace(wxT("\u2264"), mathModeStart + wxT("\\leq") + mathModeEnd);
  text.Replace(wxT("\u2265"), mathModeStart + wxT("\\geq") + mathModeEnd);
  text.Replace(wxT("\u226A"), mathModeStart + wxT("\\ll") + mathModeEnd);
  text.Replace(wxT("\u226B"), mathModeStart + wxT("\\gg") + mathModeEnd);
  text.Replace(wxT("\u220e"), mathModeStart + wxT("\\blacksquare") + mathModeEnd);
  text.Replace(wxT("\u2263"), mathModeStart + wxT("\\equiv") + mathModeEnd);
  text.Replace(wxT("\u2211"), mathModeStart + wxT("\\sum") + mathModeEnd);
  text.Replace(wxT("\u220F"), mathModeStart + wxT("\\prod") + mathModeEnd);
  text.Replace(wxT("\u2225"), mathModeStart + wxT("\\parallel") + mathModeEnd);
  text.Replace(wxT("\u27C2"), mathModeStart + wxT("\\bot") + mathModeEnd);
  text.Replace(wxT("~"), mathModeStart + wxT("\\sim ") + mathModeEnd);
  text.Replace(wxT("_"), wxT("\\_ "));
  text.Replace(wxT("$"), wxT("\\$ "));
  text.Replace(wxT("%"), wxT("\\% "));
  text.Replace(wxT("&"), wxT("\\& "));
  text.Replace(wxT("@"), mathModeStart + wxT("@") + mathModeEnd);
  text.Replace(wxT("#"), mathModeStart + wxT("\\neq") + mathModeEnd);
  text.Replace(wxT("\u00A0"), wxT("~")); // A non-breakable space
  text.Replace(wxT("<"), mathModeStart + wxT("<") + mathModeEnd);
  text.Replace(wxT(">"), mathModeStart + wxT(">") + mathModeEnd);
  text.Replace(wxT("\u219D"), mathModeStart + wxT("\\leadsto") + mathModeEnd);
  text.Replace(wxT("\u2192"), mathModeStart + wxT("\\rightarrow") + mathModeEnd);
  text.Replace(wxT("\u2794"), mathModeStart + wxT("\\longrightarrow") + mathModeEnd);

  // m_IsHidden is set for parenthesis that don't need to be shown
  if (m_isHidden || (((*m_configuration)->HidemultiplicationSign()) && m_isHidableMultSign))
  {
    // Normally in TeX the spacing between variable names following each other directly
    // is chosen to show that this is a multiplication.
    // But any use of \mathit{} will change to ordinary text spacing which means we need
    // to add a \, to show that we want to multiply the two long variable names.
    if ((text == wxT("*")) || (text == wxT("\u00B7")))
    {
      // We have a hidden multiplication sign
      if (
        // This multiplication sign is between 2 cells
              ((m_previous != NULL) && (m_next != NULL)) &&
              // These cells are two variable names
              ((m_previous->GetStyle() == TS_VARIABLE) && (m_next->GetStyle() == TS_VARIABLE)) &&
              // The variable name prior to this cell has no subscript
              (!(m_previous->ToString().Contains(wxT('_')))) &&
              // we will be using \mathit{} for the TeX outout.
              ((m_next->ToString().Length() > 1) || (m_next->ToString().Length() > 1))
              )
        text = wxT("\\, ");
      else
        text = wxT(" ");
    }
    else
      text = wxEmptyString;
  }
  else
  {
    /*
      Normally we want to draw a centered dot in this case. But if we
      are in the denominator of a d/dt or are drawing the "dx" or
      similar of an integral a centered dot looks stupid and will be
      replaced by a short space ("\,") instead. Likewise we don't want
      to begin a parenthesis with a centered dot even if this
      parenthesis does contain a product.
    */

    if (m_SuppressMultiplicationDot)
    {
      text.Replace(wxT("*"), wxT("\\, "));
      text.Replace(wxT("\u00B7"), wxT("\\, "));
    }
    else
    {
      // If we want to know if the last element was a "d" we first have to
      // look if there actually is a last element.
      if (m_previous)
      {
        if (m_previous->GetStyle() == TS_SPECIAL_CONSTANT && m_previous->ToTeX() == wxT("d"))
        {
          text.Replace(wxT("*"), wxT("\\, "));
          text.Replace(wxT("\u00B7"), wxT("\\, "));
        }
        else
        {
          text.Replace(wxT("*"), wxT("\\cdot "));
          text.Replace(wxT("\u00B7"), wxT("\\cdot "));
        }
      }
    }
  }

  if (GetStyle() == TS_GREEK_CONSTANT)
  {
    if (text == wxT("\\% alpha"))
      return wxT("\\alpha ");
    else if (text == wxT("\\% beta"))
      return wxT("\\beta ");
    else if (text == wxT("\\% gamma"))
      return wxT("\\gamma ");
    else if (text == wxT("\\% delta"))
      return wxT("\\delta ");
    else if (text == wxT("\\% epsilon"))
      return wxT("\\epsilon ");
    else if (text == wxT("\\% zeta"))
      return wxT("\\zeta ");
    else if (text == wxT("\\% eta"))
      return wxT("\\eta ");
    else if (text == wxT("\\% theta"))
      return wxT("\\theta ");
    else if (text == wxT("\\% iota"))
      return wxT("\\iota ");
    else if (text == wxT("\\% kappa"))
      return wxT("\\kappa ");
    else if (text == wxT("\\% lambda"))
      return wxT("\\lambda ");
    else if (text == wxT("\\% mu"))
      return wxT("\\mu ");
    else if (text == wxT("\\% nu"))
      return wxT("\\nu ");
    else if (text == wxT("\\% xi"))
      return wxT("\\ui ");
    else if (text == wxT("\\% omicron"))
      return wxT("\\omicron ");
    else if (text == wxT("\\% pi"))
      return wxT("\\pi ");
    else if (text == wxT("\\% rho"))
      return wxT("\\rho ");
    else if (text == wxT("\\% sigma"))
      return wxT("\\sigma ");
    else if (text == wxT("\\% tau"))
      return wxT("\\tau ");
    else if (text == wxT("\\% upsilon"))
      return wxT("\\upsilon ");
    else if (text == wxT("\\% phi"))
      return wxT("\\phi ");
    else if (text == wxT("\\% chi"))
      return wxT("\\chi ");
    else if (text == wxT("\\% psi"))
      return wxT("\\psi ");
    else if (text == wxT("\\% omega"))
      return wxT("\\omega ");
    else if (text == wxT("\\% Alpha"))
      return wxT("A");
    else if (text == wxT("\\% Beta"))
      return wxT("B");
    else if (text == wxT("\\% Gamma"))
      return wxT("\\Gamma ");
    else if (text == wxT("\\% Delta"))
      return wxT("\\Delta ");
    else if (text == wxT("\\% Epsilon"))
      return wxT("\\Epsilon ");
    else if (text == wxT("\\% Zeta"))
      return wxT("\\Zeta ");
    else if (text == wxT("\\% Eta"))
      return wxT("\\Eta ");
    else if (text == wxT("\\% Theta"))
      return wxT("\\Theta ");
    else if (text == wxT("\\% Iota"))
      return wxT("\\Iota ");
    else if (text == wxT("\\% Kappa"))
      return wxT("\\Kappa ");
    else if (text == wxT("\\% Lambda"))
      return wxT("\\Lambda ");
    else if (text == wxT("\\% Mu"))
      return wxT("\\Mu ");
    else if (text == wxT("\\% Nu"))
      return wxT("\\Nu ");
    else if (text == wxT("\\% Xi"))
      return wxT("\\ui ");
    else if (text == wxT("\\% Omicron"))
      return wxT("\\Omicron ");
    else if (text == wxT("\\% Pi"))
      return wxT("\\Pi ");
    else if (text == wxT("\\% Rho"))
      return wxT("\\Rho ");
    else if (text == wxT("\\% Sigma"))
      return wxT("\\Sigma ");
    else if (text == wxT("\\% Tau"))
      return wxT("\\Tau ");
    else if (text == wxT("\\% Upsilon"))
      return wxT("\\Upsilon ");
    else if (text == wxT("\\% Phi"))
      return wxT("\\Phi ");
    else if (text == wxT("\\% Chi"))
      return wxT("\\Chi ");
    else if (text == wxT("\\% Psi"))
      return wxT("\\Psi ");
    else if (text == wxT("\\% Omega"))
      return wxT("\\Omega ");

    return text;
  }

  if (GetStyle() == TS_SPECIAL_CONSTANT)
  {
    if (text == wxT("inf"))
      return wxT("\\infty ");
    else if (text == wxT("%e"))
      return wxT("e");
    else if (text == wxT("%i"))
      return wxT("i");
    else if (text == wxT("\\% pi"))
      return wxT("\\ensuremath{\\pi} ");
    else
      return text;
  }

  if ((GetStyle() == TS_LABEL) || (GetStyle() == TS_USERLABEL))
  {
    wxString conditionalLinebreak;
    if (m_previous) conditionalLinebreak = wxT("\\]\n\\[");
    text.Trim(true);
    wxString label = text.SubString(1, text.Length() - 2);
    text = conditionalLinebreak + wxT("\\tag{") + label + wxT("}");
    label.Replace(wxT("\\% "), wxT(""));
    // Would be a good idea, but apparently breaks mathJaX
    // text += wxT("\\label{") + label + wxT("}");
  }
  else
  {
    if (GetStyle() == TS_FUNCTION)
    {
      if (text != wxEmptyString)
        text = wxT("\\operatorname{") + text + wxT("}");
    }
    else if (GetStyle() == TS_VARIABLE)
    {
      if ((m_displayedText.Length() > 1) && (text[1] != wxT('_')))
        text = wxT("\\mathit{") + text + wxT("}");
      if (text == wxT("\\% pi"))
        text = wxT("\\ensuremath{\\pi} ");
      text.Replace(wxT("\\text{ä}"), wxT("\\text{\\textit{ä}}"));
      text.Replace(wxT("\\text{ö}"), wxT("\\text{\\textit{ö}}"));
      text.Replace(wxT("\\text{ü}"), wxT("\\text{\\textit{ü}}"));
      text.Replace(wxT("\\text{Ä}"), wxT("\\text{\\textit{Ä}}"));
      text.Replace(wxT("\\text{Ö}"), wxT("\\text{\\textit{Ö}}"));
      text.Replace(wxT("\\text{Ü}"), wxT("\\text{\\textit{Ü}}"));
    }
    else if ((GetStyle() == TS_ERROR) || (GetStyle() == TS_WARNING))
    {
      if (text.Length() > 1)
        text = wxT("\\mbox{") + text + wxT("}");
    }
    else if (GetStyle() == TS_DEFAULT)
    {
      if ((text.Length() > 2) && (text != wxT("\\,")) && (text != wxT("\\, ")))
        text = wxT("\\mbox{") + text + wxT("}");
    }
  }

  if (
          (GetStyle() != TS_FUNCTION) &&
          (GetStyle() != TS_OUTDATED) &&
          (GetStyle() != TS_VARIABLE) &&
          (GetStyle() != TS_NUMBER) &&
          (GetStyle() != TS_GREEK_CONSTANT) &&
          (GetStyle() != TS_SPECIAL_CONSTANT)
          )
    text.Replace(wxT("^"), wxT("\\textasciicircum"));

  if ((GetStyle() == TS_DEFAULT) || (GetStyle() == TS_STRING))
  {
    if (text.Length() > 1)
    {
      if (((m_forceBreakLine) || (m_breakLine)))
        //text=wxT("\\ifhmode\\\\fi\n")+text;
        text = wxT("\\mbox{}\\\\") + text;
/*      if(GetStyle() != TS_DEFAULT)
        text.Replace(wxT(" "), wxT("\\, "));*/
    }
  }

  if ((GetStyle() == TS_LABEL) || (GetStyle() == TS_USERLABEL))
    text = text + wxT(" ");

  return text;
}

wxString TextCell::ToMathML()
{
  if(m_displayedText == wxEmptyString)
    return wxEmptyString;
  wxString text = XMLescape(m_displayedText);

  if(((*m_configuration)->UseUserLabels())&&(m_userDefinedLabel != wxEmptyString))
    text = XMLescape(wxT("(") + m_userDefinedLabel + wxT(")"));

  // If we didn't display a multiplication dot we want to do the same in MathML.
  if (m_isHidden || (((*m_configuration)->HidemultiplicationSign()) && m_isHidableMultSign))
  {
    text.Replace(wxT("*"), wxT("&#8290;"));
    text.Replace(wxT("\u00B7"), wxT("&#8290;"));
    if (text != wxT ("&#8290;"))
      text = wxEmptyString;
  }
  text.Replace(wxT("*"), wxT("\u00B7"));

  switch (GetStyle())
  {
    case TS_GREEK_CONSTANT:
      text = GetGreekStringUnicode();
      break;
    case TS_SPECIAL_CONSTANT:
    {
      text = GetGreekStringUnicode();
      // The "d" from d/dt can be written as a special unicode symbol. But firefox doesn't
      // support this currently => Commenting it out.
      // if((GetStyle() == TS_SPECIAL_CONSTANT) && (text == wxT("d")))
      //   text = wxT("&#2146;");
      bool keepPercent = (*m_configuration)->CheckKeepPercent();
      if (!keepPercent)
      {
        if (text == wxT("%e"))
          text = wxT("e");
        else if (text == wxT("%i"))
          text = wxT("i");
      }
    }
    break;
  case TS_VARIABLE:
    {
      text = GetGreekStringUnicode();

      bool keepPercent = (*m_configuration)->CheckKeepPercent();

      if (!keepPercent)
      {
        if (text == wxT("%pi"))
          text = wxT("\u03C0");
      }
    }
    break;
  case TS_FUNCTION:
      text = GetGreekStringUnicode();
      if (text == wxT("inf"))
        text = wxT("\u221e");
      if((text == wxT("+")) || (text == wxT("-")) || (text == wxT("*")) || (text == wxT("/")))
        return wxT("<mo>") + text + wxT("</mo>\n");
      else
        return wxT("<mi>") + text + wxT("</mi>\n");
      break;
    case TS_NUMBER:
      return wxT("<mn>") + text + wxT("</mn>\n");
      break;

    case TS_LABEL:
    case TS_USERLABEL:
      return wxT("<mtext>") + text + wxT("</mtext></mtd><mtd>\n");
      break;

    case TS_STRING:
    default:
      if (text.StartsWith(wxT("\"")))
        return wxT("<ms>") + text + wxT("</ms>\n");
      else
        return wxT("<mo>") + text + wxT("</mo>\n");
  }

  return wxT("<mo>") + text + wxT("</mo>\n");
}

wxString TextCell::ToOMML()
{
  //Text-only lines are better handled in RTF.
  if (
          ((m_previous != NULL) && (m_previous->GetStyle() != TS_LABEL) && (!m_previous->HardLineBreak())) &&
          (HardLineBreak())
          )
    return wxEmptyString;

  // Labels are text-only.
  if ((GetStyle() == TS_LABEL) || (GetStyle() == TS_USERLABEL))
    return wxEmptyString;

  wxString text = XMLescape(m_displayedText);

  // If we didn't display a multiplication dot we want to do the same in MathML.
  if (m_isHidden || (((*m_configuration)->HidemultiplicationSign()) && m_isHidableMultSign))
  {
    text.Replace(wxT("*"), wxT("&#8290;"));
    text.Replace(wxT("\u00B7"), wxT("&#8290;"));
    if (text != wxT ("&#8290;"))
      text = wxEmptyString;
  }
  text.Replace(wxT("*"), wxT("\u00B7"));

  switch (GetStyle())
  {
    case TS_GREEK_CONSTANT:
    case TS_SPECIAL_CONSTANT:
    {
      // The "d" from d/dt can be written as a special unicode symbol. But firefox doesn't
      // support this currently => Commenting it out.
      // if((GetStyle() == TS_SPECIAL_CONSTANT) && (text == wxT("d")))
      //   text = wxT("&#2146;");
      bool keepPercent = (*m_configuration)->CheckKeepPercent();
      if (!keepPercent)
      {
        if (text == wxT("%e"))
          text = wxT("e");
        else if (text == wxT("%i"))
          text = wxT("i");
      }
    }
    /* FALLTHRU */
  case TS_VARIABLE:
    {
      if (!(*m_configuration)->CheckKeepPercent())
      {
        if (text == wxT("%pi"))
          text = wxT("\u03C0");
      }
    }
    break;
  case TS_FUNCTION:
      text = GetGreekStringUnicode();
      if (text == wxT("inf"))
        text = wxT("\u221e");
      break;
    case TS_NUMBER:
      break;

    case TS_LABEL:
    case TS_USERLABEL:
      return wxEmptyString;
      break;

    case TS_STRING:
    default:
    {
    }
  }
  text = wxT("<m:r>") + text + wxT("</m:r>\n");
  return text;
}

wxString TextCell::ToRTF()
{
  wxString retval;
  wxString text = m_displayedText;

  if (m_displayedText == wxEmptyString)
    return(wxT(" "));
  
  if(((*m_configuration)->UseUserLabels())&&(m_userDefinedLabel != wxEmptyString))
    text = wxT("(") + m_userDefinedLabel + wxT(")");
  
  text.Replace(wxT("-->"), wxT("\u2192"));
  // Needed for the output of let(a/b,a+1);
  text.Replace(wxT(" --> "), wxT("\u2192"));
  if ((GetStyle() == TS_LABEL) || (GetStyle() == TS_USERLABEL))
  {
    retval += wxString::Format(wxT("\\cf%i{"), (int) GetStyle());
    retval += RTFescape(text);
    retval += wxT("}\\cf0");
  }
  return retval;
}

wxString TextCell::ToXML()
{
  wxString tag;
  wxString flags;
  if (m_isHidden || (m_isHidableMultSign))
    tag = _T("h");
  else
    switch (GetStyle())
    {
      case TS_GREEK_CONSTANT:
        tag = _T("g");
        break;
      case TS_SPECIAL_CONSTANT:
        tag = _T("s");
        break;
      case TS_VARIABLE:
        tag = _T("v");
        break;
      case TS_FUNCTION:
        tag = _T("fnm");
        break;
      case TS_NUMBER:
        tag = _T("n");
        break;
      case TS_STRING:
        tag = _T("st");
        break;
      case TS_LABEL:
        tag = _T("lbl");
        break;
      case TS_USERLABEL:
        tag = _T("lbl");
        flags += wxT(" userdefined=\"yes\"");
        break;
      default:
        tag = _T("t");
    }

  if ((m_forceBreakLine) && (GetStyle() != TS_LABEL) && (GetStyle() != TS_USERLABEL))
    flags += wxT(" breakline=\"true\"");

  if (GetStyle() == TS_ERROR)
    flags += wxT(" type=\"error\"");

  if (GetStyle() == TS_WARNING)
    flags += wxT(" type=\"warning\"");
  
  wxString xmlstring = XMLescape(m_displayedText);
  // convert it, so that the XML configuration doesn't fail
  if(m_userDefinedLabel != wxEmptyString)
    flags += wxT(" userdefinedlabel=\"") + XMLescape(m_userDefinedLabel) + wxT("\"");

  if(m_altCopyText != wxEmptyString)
    flags += wxT(" altCopy=\"") + XMLescape(m_altCopyText) + wxT("\"");

  if(m_toolTip != wxEmptyString)
    flags += wxT(" tooltip=\"") + XMLescape(m_toolTip) + wxT("\"");

  return wxT("<") + tag + flags + wxT(">") + xmlstring + wxT("</") + tag + wxT(">");
}

wxString TextCell::GetDiffPart()
{
  return wxT(",") + m_text + wxT(",1");
}

bool TextCell::IsShortNum()
{
  if (m_next != NULL)
    return false;
  else if (m_text.Length() < 4)
    return true;
  return false;
}

void TextCell::SetAltText()
{
  m_altJs = m_alt = false;
  if ((GetStyle() == TS_DEFAULT) && m_text.StartsWith("\""))
    return;

  /// Greek characters are defined in jsMath, Windows and Unicode
  if (GetStyle() == TS_GREEK_CONSTANT)
  {
    if((*m_configuration)->Latin2Greek())
    {
      m_altJs = true;
      m_altJsText = GetGreekStringTeX();
      m_texFontname = CMMI10;
      
      m_alt = true;
      m_altText = GetGreekStringUnicode();
    }
  }

    /// Check for other symbols
  else
  {
    m_altJsText = GetSymbolTeX();
    if (m_altJsText != wxEmptyString)
    {
      if (m_text == wxT("+") || m_text == wxT("="))
        m_texFontname = CMR10;
      else if (m_text == wxT("%pi"))
        m_texFontname = CMMI10;
      else
        m_texFontname = CMSY10;
      m_altJs = true;
    }
    m_altText = GetSymbolUnicode((*m_configuration)->CheckKeepPercent());
    if (m_altText != wxEmptyString)
      m_alt = true;
// #if defined __WXMSW__
//     m_altText = GetSymbolSymbol(configuration->CheckKeepPercent());
//     if (m_altText != wxEmptyString)
//     {
//       m_alt = true;
//       m_fontname = wxT("Symbol");
//     }
// #endif
  }
}

wxString TextCell::GetGreekStringUnicode() const
{
  wxString txt(m_text);

  if (txt[0] != '%')
    txt = wxT("%") + txt;

  if (txt == wxT("%alpha"))
    return wxT("\u03B1");
  else if (txt == wxT("%beta"))
    return wxT("\u03B2");
  else if (txt == wxT("%gamma"))
    return wxT("\u03B3");
  else if (txt == wxT("%delta"))
    return wxT("\u03B4");
  else if (txt == wxT("%epsilon"))
    return wxT("\u03B5");
  else if (txt == wxT("%zeta"))
    return wxT("\u03B6");
  else if (txt == wxT("%eta"))
    return wxT("\u03B7");
  else if (txt == wxT("%theta"))
    return wxT("\u03B8");
  else if (txt == wxT("%iota"))
    return wxT("\u03B9");
  else if (txt == wxT("%kappa"))
    return wxT("\u03BA");
  else if (txt == wxT("%lambda"))
    return wxT("\u03BB");
  else if (txt == wxT("%mu"))
    return wxT("\u03BC");
  else if (txt == wxT("%nu"))
    return wxT("\u03BD");
  else if (txt == wxT("%xi"))
    return wxT("\u03BE");
  else if (txt == wxT("%omicron"))
    return wxT("\u03BF");
  else if (txt == wxT("%pi"))
    return wxT("\u03C0");
  else if (txt == wxT("%rho"))
    return wxT("\u03C1");
  else if (txt == wxT("%sigma"))
    return wxT("\u03C3");
  else if (txt == wxT("%tau"))
    return wxT("\u03C4");
  else if (txt == wxT("%upsilon"))
    return wxT("\u03C5");
  else if (txt == wxT("%phi"))
    return wxT("\u03C6");
  else if (txt == wxT("%chi"))
    return wxT("\u03C7");
  else if (txt == wxT("%psi"))
    return wxT("\u03C8");
  else if (txt == wxT("%omega"))
    return wxT("\u03C9");
  else if (txt == wxT("%Alpha"))
    return wxT("\u0391");
  else if (txt == wxT("%Beta"))
    return wxT("\u0392");
  else if (txt == wxT("%Gamma"))
    return wxT("\u0393");
  else if (txt == wxT("%Delta"))
    return wxT("\u0394");
  else if (txt == wxT("%Epsilon"))
    return wxT("\u0395");
  else if (txt == wxT("%Zeta"))
    return wxT("\u0396");
  else if (txt == wxT("%Eta"))
    return wxT("\u0397");
  else if (txt == wxT("%Theta"))
    return wxT("\u0398");
  else if (txt == wxT("%Iota"))
    return wxT("\u0399");
  else if (txt == wxT("%Kappa"))
    return wxT("\u039A");
  else if (txt == wxT("%Lambda"))
    return wxT("\u039B");
  else if (txt == wxT("%Mu"))
    return wxT("\u039C");
  else if (txt == wxT("%Nu"))
    return wxT("\u039D");
  else if (txt == wxT("%Xi"))
    return wxT("\u039E");
  else if (txt == wxT("%Omicron"))
    return wxT("\u039F");
  else if (txt == wxT("%Pi"))
    return wxT("\u03A0");
  else if (txt == wxT("%Rho"))
    return wxT("\u03A1");
  else if (txt == wxT("%Sigma"))
    return wxT("\u03A3");
  else if (txt == wxT("%Tau"))
    return wxT("\u03A4");
  else if (txt == wxT("%Upsilon"))
    return wxT("\u03A5");
  else if (txt == wxT("%Phi"))
    return wxT("\u03A6");
  else if (txt == wxT("%Chi"))
    return wxT("\u03A7");
  else if (txt == wxT("%Psi"))
    return wxT("\u03A8");
  else if (txt == wxT("%Omega"))
    return wxT("\u03A9");

  return m_text;
}

wxString TextCell::GetSymbolUnicode(bool keepPercent) const
{
  if (m_text == wxT("+"))
    return wxT("+");
  else if (m_text == wxT("="))
    return wxT("=");
  else if (m_text == wxT("inf"))
    return wxT("\u221E");
  else if (m_text == wxT("%pi"))
    return wxT("\u03C0");
  else if (m_text == wxT("<="))
    return wxT("\u2264");
  else if (m_text == wxT(">="))
    return wxT("\u2265");
  #ifndef __WXMSW__
  else if (m_text == wxT(" and "))
    return wxT(" \u22C0 ");
  else if (m_text == wxT(" or "))
    return wxT(" \u22C1 ");
  else if (m_text == wxT(" xor "))
    return wxT(" \u22BB ");
  else if (m_text == wxT(" nand "))
    return wxT(" \u22BC ");
  else if (m_text == wxT(" nor "))
    return wxT(" \u22BD ");
  else if (m_text == wxT(" implies "))
    return wxT(" \u21D2 ");
  else if (m_text == wxT(" equiv "))
    return wxT(" \u21D4 ");
  else if (m_text == wxT("not"))
    return wxT("\u00AC");
  #endif
  else if (m_text == wxT("->"))
    return wxT("\u2192");
  else if (m_text == wxT("-->"))
    return wxT("\u2794");
  // The next two ones are needed for the output of let(a/b,a+1);
  else if (m_text == wxT(" --> "))
    return wxT("\u2794");
  else if (m_text == wxT(" \u2212\u2192 "))
    return wxT("\u2794");
  /*
   else if (GetStyle() == TS_SPECIAL_CONSTANT && m_text == wxT("d"))
     return wxT("\u2202");
   */

  if (!keepPercent)
  {
    if (m_text == wxT("%e"))
      return wxT("e");
    else if (m_text == wxT("%i"))
      return wxT("i");
    else if (m_text == wxT("%pi"))
      return wxString(wxT("\u03C0"));
  }

  return wxEmptyString;
}

wxString TextCell::GetGreekStringTeX() const
{
  if (m_text == wxT("gamma"))
    return wxT("\u00C0");
  else if (m_text == wxT("zeta"))
    return wxT("\u00B0");
  else if (m_text == wxT("psi"))
    return wxT("\u00C9");

  wxString txt(m_text);
  if (txt[0] != '%')
    txt = wxT("%") + txt;

  if (txt == wxT("%alpha"))
    return wxT("\u00CB");
  else if (txt == wxT("%beta"))
    return wxT("\u00CC");
  else if (txt == wxT("%gamma"))
    return wxT("\u00CD");
  else if (txt == wxT("%delta"))
    return wxT("\u00CE");
  else if (txt == wxT("%epsilon"))
    return wxT("\u00CF");
  else if (txt == wxT("%zeta"))
    return wxT("\u00B0");
  else if (txt == wxT("%eta"))
    return wxT("\u00D1");
  else if (txt == wxT("%theta"))
    return wxT("\u00D2");
  else if (txt == wxT("%iota"))
    return wxT("\u00D3");
  else if (txt == wxT("%kappa"))
    return wxT("\u00D4");
  else if (txt == wxT("%lambda"))
    return wxT("\u00D5");
  else if (txt == wxT("%mu"))
    return wxT("\u00D6");
  else if (txt == wxT("%nu"))
    return wxT("\u00B7");
  else if (txt == wxT("%xi"))
    return wxT("\u00D8");
  else if (txt == wxT("%omicron"))
    return wxT("o");
  else if (txt == wxT("%pi"))
    return wxT("\u00D9");
  else if (txt == wxT("%rho"))
    return wxT("\u00DA");
  else if (txt == wxT("%sigma"))
    return wxT("\u00DB");
  else if (txt == wxT("%tau"))
    return wxT("\u00DC");
  else if (txt == wxT("%upsilon"))
    return wxT("\u00B5");
  else if (txt == wxT("%chi"))
    return wxT("\u00DF");
  else if (txt == wxT("%psi"))
    return wxT("\u00EF");
  else if (txt == wxT("%phi"))
    return wxT("\u0027");
  else if (txt == wxT("%omega"))
    return wxT("\u0021");
  else if (txt == wxT("%Alpha"))
    return wxT("A");
  else if (txt == wxT("%Beta"))
    return wxT("B");
  else if (txt == wxT("%Gamma"))
    return wxT("\u00C0");
  else if (txt == wxT("%Delta"))
    return wxT("\u00C1");
  else if (txt == wxT("%Epsilon"))
    return wxT("E");
  else if (txt == wxT("%Zeta"))
    return wxT("Z");
  else if (txt == wxT("%Eta"))
    return wxT("H");
  else if (txt == wxT("%Theta"))
    return wxT("\u00C2");
  else if (txt == wxT("%Iota"))
    return wxT("I");
  else if (txt == wxT("%Kappa"))
    return wxT("K");
  else if (txt == wxT("%Lambda"))
    return wxT("\u00C3");
  else if (txt == wxT("%Mu"))
    return wxT("M");
  else if (txt == wxT("%Nu"))
    return wxT("N");
  else if (txt == wxT("%Xi"))
    return wxT("\u00C4");
  else if (txt == wxT("%Omicron"))
    return wxT("O");
  else if (txt == wxT("%Pi"))
    return wxT("\u00C5");
  else if (txt == wxT("%Rho"))
    return wxT("P");
  else if (txt == wxT("%Sigma"))
    return wxT("\u00C6");
  else if (txt == wxT("%Tau"))
    return wxT("T");
  else if (txt == wxT("%Upsilon"))
    return wxT("Y");
  else if (txt == wxT("%Phi"))
    return wxT("\u00C8");
  else if (txt == wxT("%Chi"))
    return wxT("X");
  else if (txt == wxT("%Psi"))
    return wxT("\u00C9");
  else if (txt == wxT("%Omega"))
    return wxT("\u00CA");

  return wxEmptyString;
}

wxString TextCell::GetSymbolTeX() const
{
  if (m_text == wxT("inf"))
    return wxT("\u0031");
  else if (m_text == wxT("+"))
    return wxT("+");
  else if (m_text == wxT("%pi"))
    return wxT("\u00D9");
  else if (m_text == wxT("="))
    return wxT("=");
  else if (m_text == wxT("->"))
    return wxT("\u0021");
  else if (m_text == wxT(">="))
    return wxT("\u00D5");
  else if (m_text == wxT("<="))
    return wxT("\u00D4");
/*
  else if (m_text == wxT(" and "))
    return wxT(" \u005E ");
  else if (m_text == wxT(" or "))
    return wxT(" \u005F ");
  else if (m_text == wxT(" nand "))
    return wxT(" \u0022 ");
  else if (m_text == wxT(" nor "))
    return wxT(" \u0023 ");
  else if (m_text == wxT(" eq "))
    return wxT(" \u002C ");
  else if (m_text == wxT(" implies "))
    return wxT(" \u0029 ");
  else if (m_text == wxT("not"))
    return wxT("\u003A");
  else if (m_text == wxT(" xor "))
    return wxT("\u00C8");
*/

  return wxEmptyString;
}


// RegExes all TextCells share.
wxRegEx TextCell::m_unescapeRegEx(wxT("\\\\(.)"));
wxRegEx TextCell::m_roundingErrorRegEx1(wxT("\\.000000000000[0-9]+$"));
wxRegEx TextCell::m_roundingErrorRegEx2(wxT("\\.999999999999[0-9]+$"));
wxRegEx TextCell::m_roundingErrorRegEx3(wxT("\\.000000000000[0-9]+e"));
wxRegEx TextCell::m_roundingErrorRegEx4(wxT("\\.999999999999[0-9]+e"));
