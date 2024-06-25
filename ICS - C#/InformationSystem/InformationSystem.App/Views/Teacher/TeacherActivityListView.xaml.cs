using InformationSystem.App.ViewModels.Teacher;

namespace InformationSystem.App.Views.Teacher;

public partial class TeacherActivityListView
{
	public TeacherActivityListView(TeacherActivityListViewModel viewModel)
        : base(viewModel)
    {
		InitializeComponent();
	}

    private void OnFrameTapped(object sender, EventArgs e)
    {
        var frame = (Frame)sender;
        var stackLayout = (VerticalStackLayout)frame.Content;
        var fullTextLabel = (Label)stackLayout.Children[1];
        var partTextLabel = (Label)stackLayout.Children[0];

        fullTextLabel.IsVisible = !fullTextLabel.IsVisible;
        partTextLabel.IsVisible = !partTextLabel.IsVisible;
    }
}