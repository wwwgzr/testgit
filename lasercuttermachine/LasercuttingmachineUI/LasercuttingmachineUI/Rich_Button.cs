using System.Drawing;
using System.Windows.Forms;

namespace LasercuttingmachineUI
{
    public partial class Rich_Button : Button
    {
        private Image mouseDownImage;

        public Image MouseDownImage
        {
            get { return mouseDownImage; }
            set { mouseDownImage = value; }
        }
        public Rich_Button()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.Selectable, false);
        }
    }
}
