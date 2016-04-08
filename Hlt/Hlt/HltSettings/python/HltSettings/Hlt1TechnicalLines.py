class Hlt1TechnicalLines:
    """Grouping of Hlt1 technical lines

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author J. Albrecht
    @date 2012-04-18
    """

    def ActiveHlt1Lines(self):
        """Return a list of active lines."""
        lines = [
            'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2',
            'Hlt1BeamGasHighRhoVertices',
            'Hlt1Lumi',
            'Hlt1L0Any', 'Hlt1L0AnyNoSPD',
            'Hlt1MBNoBias',
            'Hlt1MBNoBiasRateLimited',
            'Hlt1ODINTechnical', 'Hlt1Tell1Error', 'Hlt1ErrorEvent',  # 'Hlt1Incident'
            'Hlt1VeloClosingMicroBias',
        ]
        return lines
