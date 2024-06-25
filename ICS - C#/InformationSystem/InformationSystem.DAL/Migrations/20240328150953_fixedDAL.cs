using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace InformationSystem.DAL.Migrations
{
    /// <inheritdoc />
    public partial class fixedDAL : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "Date",
                table: "Activities");

            migrationBuilder.RenameColumn(
                name: "Shortcut",
                table: "Subjects",
                newName: "Abbreviation");

            migrationBuilder.RenameColumn(
                name: "StartTime",
                table: "Activities",
                newName: "Start");

            migrationBuilder.RenameColumn(
                name: "EndTime",
                table: "Activities",
                newName: "End");

            migrationBuilder.AlterColumn<string>(
                name: "PhotoUrl",
                table: "Students",
                type: "TEXT",
                nullable: true,
                oldClrType: typeof(string),
                oldType: "TEXT");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.RenameColumn(
                name: "Abbreviation",
                table: "Subjects",
                newName: "Shortcut");

            migrationBuilder.RenameColumn(
                name: "Start",
                table: "Activities",
                newName: "StartTime");

            migrationBuilder.RenameColumn(
                name: "End",
                table: "Activities",
                newName: "EndTime");

            migrationBuilder.AlterColumn<string>(
                name: "PhotoUrl",
                table: "Students",
                type: "TEXT",
                nullable: false,
                defaultValue: "",
                oldClrType: typeof(string),
                oldType: "TEXT",
                oldNullable: true);

            migrationBuilder.AddColumn<DateTime>(
                name: "Date",
                table: "Activities",
                type: "TEXT",
                nullable: false,
                defaultValue: new DateTime(1, 1, 1, 0, 0, 0, 0, DateTimeKind.Unspecified));
        }
    }
}
