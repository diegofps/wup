/*
 * markovlocalization.hpp
 *
 *  Created on: Feb 28, 2015
 *      Author: diegofps
 */

#ifndef __WUP_MARKOV_LOCALIZATION
#define __WUP_MARKOV_LOCALIZATION

#include <wup/common/ranges.hpp>
#include <wup/common/array.hpp>

namespace wup {

template <typename MoveFunc>
class MarkovLocalization {
public:

	// moveFunction(src, dst, length) is a function who applies the transition
	// probabilities in src and obtains the filled array dst
	MarkovLocalization(int numStates, MoveFunc &moveFunction) :
		_numStates(numStates),
		_probStates1(new double[_numStates]),
		_probStates2(new double[_numStates]),
		_moveFunction(moveFunction),
		_firstBuffer(true),
		_noise(0.05),
		_learnrate(1.0)
	{
		clear();
	}

	void
	clear()
	{
		const double prob = 1.0 / double(_numStates);
		for (auto &i : xrange(_numStates))
			_probStates1[i] = prob;
		_probStates1[0] = 1.0;
	}

	~MarkovLocalization()
	{
		delete [] _probStates1;
		delete [] _probStates2;
	}

	void
	move()
	{
		// Aplica a função para mover as probabilidades
		double * const src = _firstBuffer ? _probStates1 : _probStates2;
		double * const dst = _firstBuffer ? _probStates2 : _probStates1;
		_firstBuffer = !_firstBuffer;

		_moveFunction(src, dst, _numStates, _learnrate);

		// Adiciona o residuo (Para tratar saltos)
//		double residuo = arrayMax(dst, _numStates) * _noise;
//		if (residuo == 0.0) residuo = 0.1;
		for (auto &i : xrange(_numStates))
//			dst[i] += residuo;
			dst[i] += _noise;

		// Normaliza os valores probabilisticos
        const double total = arr::sum(dst, _numStates);
		for (auto &i : xrange(_numStates))
			dst[i] /= total;
	}

	void
	measure(const int * const readings)
	{
		double * current = getStates();
		for (auto &i : xrange(_numStates))
			current[i] = current[i] * (1.0 - _learnrate) +
					current[i] * readings[i] * _learnrate;

        double total = arr::sum(current, _numStates);
		if (total == 0) total = 1.0;
		for (auto &i : xrange(_numStates))
			current[i] /= total;
	}

	int currentState() const
	{ return indexOfMax(getStates(), _numStates); }

	void
	bestStates(int minDistance, int &best1, int&best2, double &prob1, double &prob2) const
	{
		double * current = getStates();
		best1 = indexOfMax(getStates(), _numStates);

		const int up   = best1 + minDistance;
		const int down = best1 - minDistance;

		best2 = -1;
		for (int i=0;i<_numStates;++i)
			if ( (i <= down || i >= up) &&
					(best2 == -1 || current[best2] < current[i]))
				best2 = i;

		prob1 = current[best1];
		prob2 = current[best2];
	}

	double * getStates() const
	{ return _firstBuffer ? _probStates1 : _probStates2; }

	void setNoise(const double noise)
	{ _noise = noise; }

	void setLearnRate(const double learnrate)
	{ _learnrate = learnrate; }

private:
	int _numStates;
	double * _probStates1;
	double * _probStates2;
	MoveFunc &_moveFunction;
	int _firstBuffer;
	double _noise;
	double _learnrate;
};

} /* wup */

#endif /* __WUP_MARKOV_LOCALIZATION */
