﻿// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//  Copyright (C) 2015      Gunter Königsmann <wxMaxima@physikbuch.de>
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

#ifndef BITMAP_H
#define BITMAP_H

#include "Cell.h"
#include "GroupCell.h"

/*! Renders portions of the work sheet (including 2D maths) as bitmap.

   This is used for exporting HTML with embedded maths as bitmap
   and for putting bitmaps for the clipboard
 */
class BitmapOut
{
public:
  /*! The constructor.

    \param scale By which factor the resolution should be increased in respect
           to the default 755 DPI?
    \param configuration A pointer to the pointer to this worksheet's configuration
           storage
  */
  explicit BitmapOut(Configuration **configuration, int scale = 1);

  //! This class doesn't have a copy constructor
  BitmapOut(const BitmapOut&) = delete;
  //! This class doesn't have a = operator
  BitmapOut& operator=(const BitmapOut&) = delete;
  ~BitmapOut();

  /*! Renders tree as bitmap
    
    \param tree The list of cells that is to be rendered
    \param maxSize maxSize tells the maximum size [in square pixels] that will be rendered. 
           -1 means: No limit.

    \return true, if the bitmap could be created.
   */
  bool SetData(Cell *tree, long int maxSize = -1);

  /*! Exports this bitmap to a file

    \return The size of the bitmap in millimeters. Sizes <0 indicate that the export has failed.
   */
  wxSize ToFile(wxString file);

  //! Returns the bitmap representation of the list of cells that was passed to SetData()
  wxBitmap GetBitmap() const
  { return m_bmp; }

  //! Copies the bitmap representation of the list of cells that was passed to SetData()
  bool ToClipboard();

protected:
  void DestroyTree();

  // cppcheck-suppress functionStatic
  // cppcheck-suppress functionConst
  void RecalculateWidths();

  // cppcheck-suppress functionStatic
  // cppcheck-suppress functionConst
  void BreakLines();

  // cppcheck-suppress functionStatic
  // cppcheck-suppress functionConst
  void RecalculateHeight();

  void GetMaxPoint(int *width, int *height) const;

  // cppcheck-suppress functionStatic
  // cppcheck-suppress functionConst
  void BreakUpCells();

  bool Layout(long int maxSize = -1);

  void Draw();

  std::unique_ptr<Cell> m_tree;

  double GetRealHeight() const;

  double GetRealWidth() const;

private:
  std::unique_ptr<wxMemoryDC> m_dc;
  Configuration **m_configuration, *m_oldconfig;
  //! How many times the natural resolution do we want this bitmap to be?
  int m_scale;
  wxBitmap m_bmp;
  //! The width of the current bitmap;
  int m_width;
  //! The height of the current bitmap;
  int m_height;
  //! The resolution of the bitmap.
  wxSize m_ppi;

};

#endif // BITMAP_H
