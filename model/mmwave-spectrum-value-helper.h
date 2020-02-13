/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef MMWAVE_SPECTRUM_VALUE_HELPER_H
#define MMWAVE_SPECTRUM_VALUE_HELPER_H

#include <ns3/spectrum-value.h>
#include <vector>


namespace ns3 {


/**
 * \ingroup mmwave
 *
 * \brief This class defines all functions to create spectrum model for mmwave
 */
class MmWaveSpectrumValueHelper
{
public:

  static Ptr<const SpectrumModel> GetSpectrumModel (uint32_t numRbs, double centerFrequency, uint32_t scsPerRb, double scs);

  static Ptr<SpectrumValue> CreateTxPowerSpectralDensity (double powerTx,
                                                          std::vector <int> activeRbs,
                                                          Ptr<const SpectrumModel> spectrumModel,
                                                          double bandwidth);

  static Ptr<SpectrumValue> CreateNoisePowerSpectralDensity (double noiseFigure, Ptr<const SpectrumModel> spectrumModel);

private:
  //static Ptr<SpectrumModel> m_model;
};


} // namespace ns3



#endif /*  MMWAVE_SPECTRUM_VALUE_HELPER_H */
