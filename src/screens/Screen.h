// Joseph Arhar

class Screen {
 public:
  Screen();
  virtual ~Screen();

  virtual void Render() = 0;
  virtual void SwitchToForeground() = 0;
};
